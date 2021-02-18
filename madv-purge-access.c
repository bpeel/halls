#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <xf86drm.h>
#include <sys/mman.h>

#include "drm-uapi/vc4_drm.h"

#define DEVICE_NAME "/dev/dri/card0"

#define BUF_SIZE (32 * 1024 * 1024)

static uint32_t
create_buf(int dev_fd)
{
        struct drm_vc4_create_bo req = {
                .size = BUF_SIZE,
        };
        int ret;

        ret = drmIoctl(dev_fd, DRM_IOCTL_VC4_CREATE_BO, &req);

        if (ret == -1) {
                fprintf(stderr, "error creating buffer: %s\n", strerror(errno));
                return 0;
        }

        return req.handle;
}

static void *
map_buf(int dev_fd,
        uint32_t buf)
{
        int ret;
        void *ptr;

        struct drm_vc4_mmap_bo req = {
                .handle = buf,
        };

        ret = drmIoctl(dev_fd, DRM_IOCTL_VC4_MMAP_BO, &req);
        if (ret == -1) {
                fprintf(stderr, "vc4_mmap_bo failed: %s\n", strerror(errno));
                return false;
        }

        ptr = mmap(NULL, /* addr */
                   BUF_SIZE,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   dev_fd,
                   req.offset);

        if (ptr == NULL) {
                fprintf(stderr, "mmap failed: %s\n", strerror(errno));
                return false;
        }

        return ptr;
}

static void
destroy_buf(int dev_fd,
            uint32_t buf)
{
        struct drm_gem_close req = {
                .handle = buf,
        };

        drmIoctl(dev_fd, DRM_IOCTL_GEM_CLOSE, &req);
}

static bool
mark_as_purgeable(int dev_fd,
                  uint32_t id)
{
        struct drm_vc4_gem_madvise madv = {
                .handle = id,
                .madv = VC4_MADV_DONTNEED,
        };

        int ret = drmIoctl(dev_fd, DRM_IOCTL_VC4_GEM_MADVISE, &madv);

        if (ret == -1) {
                fprintf(stderr, "madv failed: %s\n", strerror(errno));
                return false;
        }

        return true;
}

struct buf_list {
        uint32_t id;
        struct buf_list *next;
};

static void
fill_memory(int dev_fd)
{
        /* Try to allocate buffers until one fails. This will probably
         * fill up the CMA memory and cause the userspace cache to be
         * purged.
         */

        struct buf_list *list = NULL;

        while (true) {
                uint32_t buf = create_buf(dev_fd);

                if (buf == 0)
                        break;

                struct buf_list *node = malloc(sizeof *node);

                node->id = buf;
                node->next = list;
                list = node;
        }

        for (struct buf_list *node = list, *next; node; node = next) {
                next = node->next;
                destroy_buf(dev_fd, node->id);
                free(node);
        }
}

static bool
run_test(int dev_fd)
{
        uint32_t buf = create_buf(dev_fd);
        bool ret = true;

        if (buf == 0)
                return false;

        void *buf_map = map_buf(dev_fd, buf);

        if (buf_map == NULL) {
                ret = false;
                goto out;
        }

        if (!mark_as_purgeable(dev_fd, buf)) {
                ret = false;
                goto out_map;
        }

        fill_memory(dev_fd);

        memset(buf_map, 42, BUF_SIZE);

out_map:
        munmap(buf_map, BUF_SIZE);

out:
        destroy_buf(dev_fd, buf);

        return ret;
}

int
main(int argc, char **argv)
{
        int dev_fd = open(DEVICE_NAME, O_RDWR);

        if (dev_fd == -1) {
                fprintf(stderr, "%s: %s\n", DEVICE_NAME, strerror(errno));
                return EXIT_FAILURE;
        }

        bool ret = run_test(dev_fd);

        close(dev_fd);

        return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}

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
#define BUF_LABEL "map-buf test buf"

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

static bool
set_label(int dev_fd,
          uint32_t buf)
{
        struct drm_vc4_label_bo req = {
                .handle = buf,
                .len = strlen(BUF_LABEL),
                .name = (uintptr_t) BUF_LABEL,
        };
        int ret = drmIoctl(dev_fd, DRM_IOCTL_VC4_LABEL_BO, &req);

        if (ret == -1) {
                fprintf(stderr, "label_bo failed: %s\n", strerror(errno));
                return false;
        }

        return true;
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

static void
write_pages(char *buf)
{
        /* Write to every eighth page */
        for (size_t offset = 0; offset < BUF_SIZE; offset += 4096 * 8)
                buf[offset] = 0x42;
}

static void
print_pages(char *buf)
{
        int xpos = 0;

        /* Print the first two bytes of every eighth page */
        for (size_t offset = 0; offset < BUF_SIZE; offset += 4096 * 8) {
                printf("%02x %02x ", buf[offset], buf[offset + 1]);
                if (++xpos >= 8) {
                        fputc('\n', stdout);
                        xpos = 0;
                }
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

        write_pages(buf_map);

        if (!set_label(dev_fd, buf)) {
                ret = false;
                goto out_map;
        }

        print_pages(buf_map);

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

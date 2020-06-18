#include <epoxy/gl.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define N_VERTS 4

static const struct {
        GLenum type;
        const char *filename;
} shader_names[] = {
        { GL_VERTEX_SHADER, "test-regs-vertex.shader" },
        { GL_FRAGMENT_SHADER, "test-regs-fragment.shader" },
};

#define N_SHADERS ((sizeof shader_names) / (sizeof shader_names[0]))

struct data {
        SDL_Window *window;
        SDL_GLContext gl_context;

        GLuint program;
        GLuint vao, vbo;
        GLuint tex;

        bool quit;
        bool redraw_queued;
};

static SDL_GLContext
create_gl_context(SDL_Window *window)
{
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_ES);

        return SDL_GL_CreateContext(window);
}

static void
handle_event(struct data *data,
             const SDL_Event *event)
{
        switch (event->type) {
        case SDL_QUIT:
                data->quit = true;
                break;
        case SDL_WINDOWEVENT:
                switch (event->window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                        data->quit = true;
                        break;
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_EXPOSED:
                        data->redraw_queued = true;
                        break;
                }
                break;
        }
}

static void
handle_redraw(struct data *data)
{
        int w, h;

        SDL_GetWindowSize(data->window, &w, &h);
        glViewport(0, 0, w, h);

        glBindVertexArray(data->vao);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERTS);

        uint8_t *pixels = malloc(w * 4);
        glReadPixels(0, 0, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        for (int i = 0; i < w; i++)
                printf("%02x ", pixels[i]);
        fputc('\n', stdout);
        free(pixels);

        SDL_GL_SwapWindow(data->window);

        data->redraw_queued = false;
}

static void
run_main_loop(struct data *data)
{
        SDL_Event event;
        bool had_event;

        while (!data->quit) {
                if (data->redraw_queued)
                        had_event = SDL_PollEvent(&event);
                else
                        had_event = SDL_WaitEvent(&event);

                if (had_event)
                        handle_event(data, &event);
                else if (data->redraw_queued)
                        handle_redraw(data);
        }
}

static bool
get_shader_source(const char *filename,
                  GLint *length_out,
                  char **source_out)
{
        struct stat statbuf;

        if (stat(filename, &statbuf) == -1) {
                fprintf(stderr, "%s: %s\n", filename, strerror(errno));
                return false;
        }

        FILE *f = fopen(filename, "r");

        if (f == NULL) {
                fprintf(stderr, "%s: %s\n", filename, strerror(errno));
                return false;
        }

        char *source = malloc(statbuf.st_size);

        size_t got = fread(source, 1, statbuf.st_size, f);

        fclose(f);

        if (got != statbuf.st_size) {
                fprintf(stderr, "error reading “%s”\n", filename);
                free(source);
                return false;
        }

        *length_out = statbuf.st_size;
        *source_out = source;

        return true;
}

static GLuint
create_shader(GLenum type,
              const char *filename)
{
        char *source;
        GLint source_length;

        if (!get_shader_source(filename, &source_length, &source))
                return 0;

        GLuint shader = glCreateShader(type);;

        shader = glCreateShader(type);

        glShaderSource(shader,
                       1, /* n_strings */
                       (const GLchar * const*) &source,
                       &source_length);

        glCompileShader(shader);

        GLint length;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
                GLsizei actual_length;
                GLchar *info_log = malloc(length);

                glGetShaderInfoLog(shader,
                                   length,
                                   &actual_length,
                                   info_log);
                if (*info_log)
                        fprintf(stderr,
                                "Info log for %s:\n%s\n",
                                filename, info_log);
                free(info_log);
        }

        GLint compile_status;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

        if (!compile_status) {
                fprintf(stderr, "%s compilation failed\n", filename);
                glDeleteShader(shader);
                return 0;
        }

        return shader;
}

static bool
link_program(GLuint program)
{
        GLint length, link_status;
        GLsizei actual_length;
        GLchar *info_log;

        glLinkProgram(program);

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
                info_log = malloc(length);
                glGetProgramInfoLog(program, length,
                                    &actual_length,
                                    info_log);
                if (*info_log) {
                        fprintf(stderr, "Link info log:\n%s\n",
                                info_log);
                }
                free(info_log);
        }

        glGetProgramiv(program, GL_LINK_STATUS, &link_status);

        if (!link_status) {
                fprintf(stderr, "program link failed\n");
                return false;
        }

        return true;
}

static void
delete_shaders(size_t n_shaders, GLuint *shaders)
{
        for (unsigned i = 0; i < n_shaders; i++)
                glDeleteShader(shaders[i]);
}

static bool
init_program(struct data *data)
{
        GLuint shaders[N_SHADERS];

        for (unsigned i = 0; i < N_SHADERS; i++) {
                GLuint shader = create_shader(shader_names[i].type,
                                              shader_names[i].filename);
                if (shader == 0) {
                        delete_shaders(i, shaders);
                        return false;
                }

                shaders[i] = shader;
        }

        GLuint prog = glCreateProgram();

        for (unsigned i = 0; i < N_SHADERS; i++) {
                glAttachShader(prog, shaders[i]);
                glDeleteShader(shaders[i]);
        }

        glBindAttribLocation(prog, 0, "pos");

        if (!link_program(prog)) {
                glDeleteProgram(prog);
                return false;
        }

        glUseProgram(prog);

        GLint loc = glGetUniformLocation(prog, "tex");
        glUniform1i(loc, 0);

        data->program = prog;

        return true;
}

static void
init_vertices(struct data *data)
{
        GLfloat verts[N_VERTS * 2], *p = verts;

        *(p++) = -1.0f;
        *(p++) = -1.0f;
        *(p++) = 1.0f;
        *(p++) = -1.0f;
        *(p++) = -1.0f;
        *(p++) = 1.0f;
        *(p++) = 1.0f;
        *(p++) = 1.0f;

        assert((char *) p == (char *) verts + sizeof verts);

        glGenVertexArrays(1, &data->vao);
        glBindVertexArray(data->vao);

        glGenBuffers(1, &data->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, /* index */
                              2, /* size */
                              GL_FLOAT,
                              GL_FALSE, /* normalized */
                              sizeof (GLfloat) * 2,
                              (GLvoid *) (GLintptr) 0 /* offset */);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
}

static void
deinit_vertices(struct data *data)
{
        glDeleteVertexArrays(1, &data->vao);
        glDeleteBuffers(1, &data->vbo);
}

static void
init_image(struct data *data)
{
        uint8_t buf[256 * 3];

        for (unsigned i = 0; i < (sizeof buf) / 3; i++)
                memset(buf + i * 3, i, 3);

        glGenTextures(1, &data->tex);
        glBindTexture(GL_TEXTURE_2D, data->tex);

        glTexImage2D(GL_TEXTURE_2D,
                     0, /* level */
                     GL_RGB,
                     256, /* width */
                     1, /* height */
                     0, /* border */
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     buf);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        GL_REPEAT);
}

static void
deinit_image(struct data *data)
{
        glDeleteTextures(1, &data->tex);
}

int
main(int argc, char **argv)
{
        struct data data;
        int ret = EXIT_SUCCESS;
        int res;

        memset(&data, 0, sizeof data);

        res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
        if (res < 0) {
                fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
                ret = EXIT_FAILURE;
                goto out;
        }

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        data.window = SDL_CreateWindow("Halls",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       800, 600,
                                       SDL_WINDOW_OPENGL |
                                       SDL_WINDOW_RESIZABLE);
        if (data.window == NULL) {
                fprintf(stderr,
                        "Failed to create SDL window: %s\n",
                        SDL_GetError());
                ret = EXIT_FAILURE;
                goto out_sdl;
        }

        data.gl_context = create_gl_context(data.window);
        if (data.gl_context == NULL) {
                fprintf(stderr,
                        "Failed to create GL context: %s\n",
                        SDL_GetError());
                ret = EXIT_FAILURE;
                goto out_window;
        }

        SDL_GL_MakeCurrent(data.window, data.gl_context);

        if (!init_program(&data)) {
                ret = EXIT_FAILURE;
                goto out_context;
        }

        init_image(&data);

        init_vertices(&data);

        run_main_loop(&data);

        deinit_vertices(&data);

        deinit_image(&data);

        glUseProgram(0);
        glDeleteProgram(data.program);

out_context:
        SDL_GL_MakeCurrent(NULL, NULL);
        SDL_GL_DeleteContext(data.gl_context);
out_window:
        SDL_DestroyWindow(data.window);
out_sdl:
        SDL_Quit();
out:
        return ret;
}

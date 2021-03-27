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
#include <math.h>
#include <assert.h>

#define TEX_SIZE 2048
#define TEX_HEIGHT 208
#define FB_WIDTH 800
#define FB_HEIGHT 600

/* Use 128MB with all the textures */
#define TEXTURE_BYTES (TEX_SIZE * TEX_SIZE * 4)
#define N_TEXTURES (128 * 1024 * 1024 / TEXTURE_BYTES)
#define TEXTURES_PER_DRAW (N_TEXTURES / 2)
#define N_QUADS (N_TEXTURES / TEXTURES_PER_DRAW)
#define N_VERTS (N_QUADS * 4)

static const struct {
        GLenum type;
        const char *filename;
} shader_names[] = {
        { GL_VERTEX_SHADER, "huge-vertex.shader" },
        { GL_FRAGMENT_SHADER, "huge-fragment.shader" },
};

#define N_SHADERS ((sizeof shader_names) / (sizeof shader_names[0]))

struct data {
        SDL_Window *window;
        SDL_GLContext gl_context;

        GLuint program;
        GLuint vbo;
        GLuint textures[N_TEXTURES];

        uint8_t *texture_data;

        int draw_num;

        bool quit;
        bool redraw_queued;
        bool delay_create;
};

static SDL_GLContext
create_gl_context(SDL_Window *window)
{
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        return SDL_GL_CreateContext(window);
}

static GLuint
create_texture(int width,
               int height,
               const uint8_t *tex_data)
{
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D,
                     0, /* level */
                     GL_RGBA,
                     width,
                     height,
                     0, /* border */
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     tex_data);

        return tex;
}

static void
init_texture(struct data *data,
             int tex_num)
{
        if (data->textures[tex_num])
                return;

        data->textures[tex_num] = create_texture(TEX_SIZE,
                                                 TEX_SIZE,
                                                 data->texture_data +
                                                 tex_num * TEXTURE_BYTES);
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
        case SDL_KEYDOWN:
                switch (event->key.keysym.sym) {
                case SDLK_SPACE:
                        data->redraw_queued = true;
                        break;
                }
                break;
        case SDL_MOUSEBUTTONDOWN:
                data->redraw_queued = true;
                break;
        }
}

static void
handle_redraw(struct data *data)
{
        int w, h;

        if (data->delay_create) {
                glDeleteTextures(N_TEXTURES, data->textures);
                memset(data->textures, 0, sizeof data->textures);
        }

        SDL_GetWindowSize(data->window, &w, &h);
        glViewport(0, 0, w, h);

        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < N_TEXTURES / TEXTURES_PER_DRAW; i++) {
                for (int j = 0; j < TEXTURES_PER_DRAW; j++) {
                        int tex_num = ((i * TEXTURES_PER_DRAW + j +
                                        data->draw_num) %
                                       N_TEXTURES);

                        glActiveTexture(GL_TEXTURE0 + j);

                        init_texture(data, tex_num);

                        glBindTexture(GL_TEXTURE_2D, data->textures[tex_num]);
                }
                glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
        }

        SDL_GL_SwapWindow(data->window);

        data->redraw_queued = false;
        data->draw_num++;
}

static void
run_main_loop(struct data *data)
{
        SDL_Event event;
        bool had_event;

        while (!data->quit) {
                if (data->redraw_queued) {
                        had_event = SDL_PollEvent(&event);
                } else {
                        had_event = SDL_WaitEvent(&event);
                }

                if (had_event)
                        handle_event(data, &event);
                else if (data->redraw_queued)
                        handle_redraw(data);
        }
}

static bool
get_file_contents(const char *filename,
                  GLint *length_out,
                  char **contents_out)
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

        char *contents = malloc(statbuf.st_size);

        size_t got = fread(contents, 1, statbuf.st_size, f);

        fclose(f);

        if (got != statbuf.st_size) {
                fprintf(stderr, "error reading “%s”\n", filename);
                free(contents);
                return false;
        }

        *length_out = statbuf.st_size;
        *contents_out = contents;

        return true;
}

static GLuint
create_shader(GLenum type,
              const char *filename)
{
        char *source;
        GLint source_length;

        if (!get_file_contents(filename, &source_length, &source))
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

        static const char name[] = "tex";
        GLint tex_uniform = glGetUniformLocation(prog, name);

        if (tex_uniform == -1) {
                fprintf(stderr, "missing “%s” uniform\n", name);
                glDeleteProgram(prog);
                return false;
        }

        for (int i = 0; i < TEXTURES_PER_DRAW; i++)
                glUniform1i(tex_uniform + i, i);

        data->program = prog;

        return true;
}

static void
init_one_texture_data(uint8_t *tex_data,
                      uint32_t color)
{
        color = SDL_SwapBE32(color);

        for (unsigned i = 0; i < TEX_SIZE * TEX_SIZE; i++)
                memcpy(tex_data + i * 4, &color, sizeof color);
}

static void
init_texture_data(struct data *data)
{
        data->texture_data = malloc(TEXTURE_BYTES * N_TEXTURES);

        for (int i = 0; i < N_TEXTURES; i++) {
                int cnum = i % 7 + 1;

                init_one_texture_data(data->texture_data + TEXTURE_BYTES * i,
                                      ((cnum & 1) * 0xff00) |
                                      ((cnum & 2) * 0x7f8000) |
                                      ((cnum & 4) * 0x3fc00000) |
                                      0xff);
        }
}

static void
init_vertices(struct data *data)
{
        GLfloat verts[N_VERTS * 2];

        for (int i = 0; i < N_QUADS; i++) {
                float l = 2 * i / (float) N_QUADS - 1.0;
                float r = l + 2 / (float) N_QUADS;
                verts[i * 8 + 0] = l;
                verts[i * 8 + 1] = -0.6;
                verts[i * 8 + 2] = r;
                verts[i * 8 + 3] = -1;
                verts[i * 8 + 4] = l;
                verts[i * 8 + 5] = 0.6;
                verts[i * 8 + 6] = r;
                verts[i * 8 + 7] = 1;
        };

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
}

static void
deinit_vertices(struct data *data)
{
        glDeleteBuffers(1, &data->vbo);
}

static void
init_textures(struct data *data)
{
        for (int i = 0; i < N_TEXTURES; i++)
                init_texture(data, i);
}

static void
deinit_textures(struct data *data)
{
        glDeleteTextures(N_TEXTURES, data->textures);
}

int
main(int argc, char **argv)
{
        struct data data;
        int ret = EXIT_SUCCESS;
        int res;

        memset(&data, 0, sizeof data);

        for (int i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "delaycreate") ||
                    !strcmp(argv[i], "createdelay")) {
                        data.delay_create = true;
                } else {
                        fprintf(stderr, "usage: %s [delaycreate]\n", argv[0]);
                        ret = EXIT_FAILURE;
                        goto out;
                }
        }

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

        data.window = SDL_CreateWindow("Huge texture MS",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       FB_WIDTH, FB_HEIGHT,
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

        init_texture_data(&data);

        if (!data.delay_create)
                init_textures(&data);

        init_vertices(&data);

        run_main_loop(&data);

        deinit_vertices(&data);

        deinit_textures(&data);

        free(data.texture_data);

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

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

/* Padding around the grid of lines */
#define EDGE_PADDING 0.1f
/* Padding in-between the lines */
#define LINE_PADDING 0.2f

#define N_LINES_X 4
#define N_LINES_Y 3
#define N_LINES (N_LINES_X * N_LINES_Y)

#define LINE_WIDTH ((2.0f -                             \
                     EDGE_PADDING * 2 -                 \
                     LINE_PADDING * (N_LINES_X - 1)) /  \
                    N_LINES_X)
#define LINE_HEIGHT ((2.0f -                            \
                      EDGE_PADDING * 2 -                \
                      LINE_PADDING * (N_LINES_Y - 1)) / \
                     N_LINES_Y)

struct data {
        SDL_Window *window;
        SDL_GLContext gl_context;

        GLuint program;
        GLuint vao, vbo;

        bool quit;
        bool redraw_queued;
};

static SDL_GLContext
create_gl_context(SDL_Window *window)
{
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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

        glLineWidth(8.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(data->program);
        glBindVertexArray(data->vao);
        glDrawArrays(GL_LINES, 0, N_LINES * 2);
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

static bool
init_program(struct data *data)
{
        GLuint vert_shader = create_shader(GL_VERTEX_SHADER,
                                           "test-lines-vertex.shader");
        if (vert_shader == 0)
                return false;

        GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER,
                                           "test-lines-fragment.shader");
        if (frag_shader == 0) {
                glDeleteShader(vert_shader);
                return false;
        }

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vert_shader);
        glAttachShader(prog, frag_shader);

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

        glBindAttribLocation(prog, 0, "pos");

        if (!link_program(prog)) {
                glDeleteProgram(prog);
                return false;
        }

        glUseProgram(prog);

        data->program = prog;

        return true;
}

static void
init_vertices(struct data *data)
{
        GLfloat verts[N_LINES * 4];
        float *p = verts;

        for (int y = 0; y < N_LINES_Y; y++) {
                for (int x = 0; x < N_LINES_X; x++) {
                        p[0] = (EDGE_PADDING - 1.0f +
                                x * (LINE_WIDTH + LINE_PADDING));
                        p[1] = (EDGE_PADDING - 1.0f +
                                y * (LINE_HEIGHT + LINE_PADDING));
                        p[2] = p[0] + LINE_WIDTH;
                        p[3] = p[1] + LINE_HEIGHT;
                        p += 4;
                }
        }

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

        init_vertices(&data);

        run_main_loop(&data);

        deinit_vertices(&data);

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

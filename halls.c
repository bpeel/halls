#include <epoxy/gl.h>
#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define CORE_GL_MAJOR_VERSION 3
#define CORE_GL_MINOR_VERSION 3

static const char
vert_shader_source[] =
        "#version 330\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void\n"
        "main()\n"
        "{\n"
        "        gl_Position = position;\n"
        "}\n";

static const char
frag_shader_source[] =
        "#version 330\n"
        "\n"
        "out vec4 color;\n"
        "\n"
        "void\n"
        "main()\n"
        "{\n"
        "        color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                            CORE_GL_MAJOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                            CORE_GL_MINOR_VERSION);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

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
        glUseProgram(data->program);
        glBindVertexArray(data->vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

static GLuint
create_shader(const char *name,
              GLenum type,
              const char *source,
              int source_length)
{
        GLuint shader;
        GLint length, compile_status;
        GLsizei actual_length;
        GLchar *info_log;

        shader = glCreateShader(type);

        glShaderSource(shader,
                       1, /* n_strings */
                       &source,
                       &source_length);

        glCompileShader(shader);

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        if (length > 0) {
                info_log = malloc(length);
                glGetShaderInfoLog(shader,
                                   length,
                                   &actual_length,
                                   info_log);
                if (*info_log)
                        fprintf(stderr,
                                "Info log for %s:\n%s\n",
                                name, info_log);
                free(info_log);
        }

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

        if (!compile_status) {
                fprintf(stderr, "%s compilation failed\n", name);
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
        GLuint vert_shader, frag_shader;
        GLuint prog;

        vert_shader = create_shader("vert shader",
                                    GL_VERTEX_SHADER,
                                    vert_shader_source,
                                    sizeof vert_shader_source - 1);
        if (vert_shader == 0)
                return false;

        frag_shader = create_shader("frag shader",
                                    GL_FRAGMENT_SHADER,
                                    frag_shader_source,
                                    sizeof frag_shader_source - 1);
        if (frag_shader == 0) {
                glDeleteShader(vert_shader);
                return false;
        }

        prog = glCreateProgram();
        glAttachShader(prog, vert_shader);
        glAttachShader(prog, frag_shader);

        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);

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
        static const GLfloat verts[] = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f
        };

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
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        data.window = SDL_CreateWindow("Finvenkisto",
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

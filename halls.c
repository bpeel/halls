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
        "#define tex_coords webgl_51798873aa8ff428\n"
        "out vec2 tex_coords;\n"
        "\n"
        "void\n"
        "main()\n"
        "{\n"
        "        gl_Position = position;\n"
        "        tex_coords = vec2(position.x, -position.y) / 2.0 + 0.5;\n"
        "}\n";

static const char
frag_shader_source[] =
        "#version 130\n"
        "out vec4 webgl_FragColor;\n"
        "uniform mat4 webgl_2f0d57c31e3ad65f;\n"
        "uniform vec3 webgl_fa78a8ce74d1aaee;\n"
        "uniform float webgl_9f3d75d0c870cd0a;\n"
        "uniform float webgl_73f8cb2ab947f459;\n"
        "uniform float webgl_58eb32532f5ee481;\n"
        "uniform float webgl_e05f6d076f9b3f32;\n"
        "uniform float webgl_53848c7f1cf694ac;\n"
        "uniform float webgl_b23f3037257feb4b;\n"
        "uniform float webgl_da6e391939e86900;\n"
        "uniform float webgl_982623471b1d160e;\n"
        "uniform float webgl_c9cd165c64c355df;\n"
        "uniform vec3 webgl_77bf37367d0ad46d;\n"
        "uniform sampler2D webgl_3543ec1d70156653;\n"
        "in vec2 webgl_51798873aa8ff428;\n"
        "void main(){\n"
        "vec4 webgl_19dff938713edbff, webgl_a8f0f678f99829f9, "
        "webgl_86be61d1965e63b7, webgl_48c57da858454f79;\n"
        "float webgl_55fdf3cc830b6c33, webgl_a34964a2a96b201f;\n"
        "vec2 webgl_cb128f6fdfe0160b;\n"
        "vec2 webgl_758cdc8b0df6c2a9 = webgl_51798873aa8ff428;\n"
        "(webgl_48c57da858454f79 += (webgl_19dff938713edbff = "
        "(webgl_a8f0f678f99829f9 = texture(webgl_3543ec1d70156653, "
        "webgl_758cdc8b0df6c2a9))));\n"
        "(webgl_cb128f6fdfe0160b = ((webgl_758cdc8b0df6c2a9 - vec2(0.5, 0.5)) "
        "* vec2(1.55, 1.55)));\n"
        "(webgl_55fdf3cc830b6c33 = (dot(webgl_cb128f6fdfe0160b, "
        "webgl_cb128f6fdfe0160b) * 2.0));\n"
        "(webgl_a34964a2a96b201f = ((((webgl_c9cd165c64c355df * 100.0) + "
        "webgl_55fdf3cc830b6c33) * webgl_da6e391939e86900) * 4.0));\n"
        "vec2 webgl_ebe00033516781cb = (vec2((1.0 / webgl_9f3d75d0c870cd0a), "
        "(1.0 / webgl_73f8cb2ab947f459)) * vec2(webgl_a34964a2a96b201f));\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(0.111964, 0.99371201) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(0.84672397, 0.53203201) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(0.943883, -0.33027899) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(0.33027899, -0.943883) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(-0.53203201, -0.84672397) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(-0.99371201, -0.111964) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_48c57da858454f79 += (webgl_86be61d1965e63b7 = "
        "texture(webgl_3543ec1d70156653, (webgl_758cdc8b0df6c2a9 + "
        "(vec2(-0.70710701, 0.70710701) * webgl_ebe00033516781cb)))));\n"
        "if ((webgl_86be61d1965e63b7.y > webgl_19dff938713edbff.y))\n"
        "{\n"
        "(webgl_19dff938713edbff = webgl_86be61d1965e63b7);\n"
        "}\n"
        "(webgl_758cdc8b0df6c2a9 = ((webgl_758cdc8b0df6c2a9 - "
        "vec2(0.5, 0.5)) * vec2(webgl_58eb32532f5ee481)));\n"

        /* Delete this section to replicate the fix in the bug report */
        "(webgl_19dff938713edbff = (webgl_19dff938713edbff + "
        "(((webgl_48c57da858454f79 / vec4(8.0, 8.0, 8.0, 8.0)) - "
        "webgl_19dff938713edbff) * (vec4(1.0, 1.0, 1.0, 1.0) - "
        "vec4((webgl_55fdf3cc830b6c33 * 0.5))))));\n"

        "(webgl_FragColor = vec4(mix((((webgl_19dff938713edbff.xyz * "
        "webgl_19dff938713edbff.xyz) * vec3(webgl_53848c7f1cf694ac)) + "
        "webgl_19dff938713edbff.xyz), ((webgl_19dff938713edbff.yyy * "
        "webgl_b23f3037257feb4b) - vec3(webgl_e05f6d076f9b3f32)), "
        "vec3(dot(webgl_758cdc8b0df6c2a9, webgl_758cdc8b0df6c2a9))), 1.0));\n"
        "(webgl_FragColor = (vec4(1.0, 1.0, 1.0, 1.0) - "
        "((vec4(1.0, 1.0, 1.0, 1.0) - webgl_FragColor) * "
        "(vec4(1.0, 1.0, 1.0, 1.0) - webgl_FragColor))));\n"
        "}\n";

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
        int w, h;

        SDL_GetWindowSize(data->window, &w, &h);
        glViewport(0, 0, w, h);

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
        GLint loc;

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

        loc = glGetUniformLocation(prog, "webgl_11d5c59b099a10a2");
        glUniformMatrix4fv(loc,
                           1, /* count */
                           GL_FALSE, /* transpose */
                           (GLfloat[]) { 0.001953125, 0, 0, 0, 0, 0.003472222,
                                           0, 0, 0, 0, -0.0001, 0, -0, -0, -0,
                                           1 });
        loc = glGetUniformLocation(prog, "webgl_f4376ea35a7e1f46");
        glUniform1i(loc, 0);
        loc = glGetUniformLocation(prog, "webgl_9f3d75d0c870cd0a");
        glUniform1fv(loc, 1, (GLfloat[]) { 1024 });
        loc = glGetUniformLocation(prog, "webgl_73f8cb2ab947f459");
        glUniform1fv(loc, 1, (GLfloat[]) { 576 });
        loc = glGetUniformLocation(prog, "webgl_58eb32532f5ee481");
        glUniform1fv(loc, 1, (GLfloat[]) { 0.94 });
        loc = glGetUniformLocation(prog, "webgl_e05f6d076f9b3f32");
        glUniform1fv(loc, 1, (GLfloat[]) { 0.66 });
        loc = glGetUniformLocation(prog, "webgl_53848c7f1cf694ac");
        glUniform1fv(loc, 1, (GLfloat[]) { 0 });
        loc = glGetUniformLocation(prog, "webgl_b23f3037257feb4b");
        glUniform1fv(loc, 1, (GLfloat[]) { 1 });
        loc = glGetUniformLocation(prog, "webgl_da6e391939e86900");
        glUniform1fv(loc, 1, (GLfloat[]) { 0.3 });
        loc = glGetUniformLocation(prog, "webgl_c9cd165c64c355df");
        glUniform1fv(loc, 1, (GLfloat[]) { 0.00161 });
        loc = glGetUniformLocation(prog, "webgl_f4376ea35a7e1f46");
        glUniformMatrix4fv(loc,
                           1, /* count */
                           GL_FALSE, /* transpose */
                           (GLfloat[]) { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                           1, 0, 0, 0, -600, 1 });

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

static bool
init_image(struct data *data)
{
        SDL_Surface *surface;
        int alignment = 1;

        surface = SDL_LoadBMP("image.bmp");
        if (surface == NULL) {
                fprintf(stderr, "image.bmp: %s\n", SDL_GetError());
                return false;
        }

        if (surface->format->format != SDL_PIXELFORMAT_BGR24) {
                fprintf(stderr, "Unknown pixel format in image: %s\n",
                        SDL_GetPixelFormatName(surface->format->format));
                goto error;
        }

        glGenTextures(1, &data->tex);
        glBindTexture(GL_TEXTURE_2D, data->tex);
        glPixelStorei(GL_UNPACK_ROW_LENGTH,
                      surface->pitch / surface->format->BytesPerPixel);
        while (alignment < surface->pitch % surface->format->BytesPerPixel)
                alignment *= 2;
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        glTexImage2D(GL_TEXTURE_2D,
                     0, /* level */
                     GL_RGB,
                     surface->w,
                     surface->h,
                     0, /* border */
                     GL_BGR,
                     GL_UNSIGNED_BYTE,
                     surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);

        SDL_FreeSurface(surface);

        return true;

error:
        SDL_FreeSurface(surface);
        return false;
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

        if (!init_image(&data)) {
                ret = EXIT_FAILURE;
                goto out_program;
        }

        init_vertices(&data);

        run_main_loop(&data);

        deinit_vertices(&data);

        deinit_image(&data);

out_program:
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

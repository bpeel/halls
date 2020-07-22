#version 310 es

mediump layout(location = 0) out vec4 color_out;

mediump in vec3 color_fs;

void
main()
{
        color_out = vec4(color_fs, 1.0);
}

#version 310 es

mediump layout(location = 0) out vec4 color_out;

in highp vec4 fs_color;

void
main()
{
        color_out = fs_color;
}

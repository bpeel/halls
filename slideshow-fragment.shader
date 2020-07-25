#version 310 es

mediump layout(location = 0) out vec4 color_out;

mediump in vec2 tex_coord;

uniform sampler2D tex;

void
main()
{
        color_out = texture2D(tex, tex_coord);
}

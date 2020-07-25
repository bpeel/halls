#version 310 es

layout(location = 0) in vec2 pos;

mediump out vec2 tex_coord;

void
main()
{
        gl_Position = vec4(pos, 0.0, 1.0);
        tex_coord = pos.xy / 2.0 + 0.5;
}

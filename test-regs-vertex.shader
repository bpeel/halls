#version 310 es

layout(location = 0) in vec4 pos;

mediump out float x_coord;

void
main()
{
        gl_Position = pos;
        x_coord = pos.x * 0.5 + 0.5;
}

#version 310 es

layout(location = 0) in vec2 pos;

out vec4 gs_color;

void
main()
{
        gl_Position = vec4(pos, 0.0, 1.0);
        gs_color = vec4(pos.x * 0.5 + 0.5,
                        pos.y * 0.5 + 0.5,
                        length(pos),
                        1.0);
}

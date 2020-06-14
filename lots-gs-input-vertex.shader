#version 310 es

layout(location = 0) in vec4 pos;

out vec4 gs_color;

void
main()
{
        gl_Position = pos;
        gs_color = vec4(pos.x * 0.5 + 0.5,
                        pos.y * 0.5 + 0.5,
                        length(pos.xy),
                        1.0);
}

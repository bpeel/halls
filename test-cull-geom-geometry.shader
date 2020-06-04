#version 310 es
#extension GL_OES_geometry_shader : require

layout(lines) in;
layout(triangle_strip, max_vertices = 3) out;

void
main()
{
        /* Skip every odd triangle */
        if ((gl_PrimitiveIDIn & 1) != 0)
                return;

        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = vec4((gl_in[0].gl_Position.xy +
                            gl_in[1].gl_Position.xy) / 2.0 *
                           0.5,
                           0.0,
                           1.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();

        EndPrimitive();
}

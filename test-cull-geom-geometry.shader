#version 310 es
#extension GL_OES_geometry_shader : require

layout(lines) in;
layout(triangle_strip, max_vertices = 7) out;

void
main()
{
        if ((gl_PrimitiveIDIn & 2) == 0)
                return;

        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();

        gl_Position = vec4(gl_in[1].gl_Position.xy * 0.5, 0.0, 1.0);
        EmitVertex();
        gl_Position = vec4(gl_in[0].gl_Position.xy * 0.5, 0.0, 1.0);
        EmitVertex();

        EndPrimitive();

        if ((gl_PrimitiveIDIn & 1) == 0) {
                gl_Position = vec4(gl_in[1].gl_Position.xy * 0.4, 0.0, 1.0);
                EmitVertex();
                gl_Position = vec4(gl_in[0].gl_Position.xy * 0.4, 0.0, 1.0);
                EmitVertex();

                gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
                EmitVertex();
        }
}

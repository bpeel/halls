#version 310 es
#extension GL_OES_geometry_shader : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void
main()
{
        if (gl_in[0].gl_Position.y > 0.0)
                return;

        for (int i = 0; i < 3; i++) {
                gl_Position = gl_in[i].gl_Position;
                EmitVertex();
        }

        EndPrimitive();
}

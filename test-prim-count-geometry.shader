#version 310 es
#extension GL_OES_geometry_shader : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

mediump out vec3 color_fs;

#define TRIANGLE_SIZE (2.0f / 7.0f)

void
main()
{
        for (int i = 0; i < 3; i++) {
                gl_Position = gl_in[i].gl_Position;
                color_fs = vec3(1.0);
                EmitVertex();
        }

        EndPrimitive();

        /* Copy the triangle offsetted with a different colour */
        for (int i = 0; i < 3; i++) {
                gl_Position = vec4(gl_in[i].gl_Position.xy +
                                   vec2(TRIANGLE_SIZE / 2.0),
                                   gl_in[i].gl_Position.zw);
                color_fs = vec3(1.0, 0.0, 0.0);
                EmitVertex();
        }
}

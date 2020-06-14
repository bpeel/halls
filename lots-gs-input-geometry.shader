#version 310 es
#extension GL_OES_geometry_shader : require

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 18) out;

in highp vec4 gs_color[];
out highp vec4 fs_color;

const float vert_offset = 0.2;

void
main()
{
        for (int i = 0; i < gl_in.length(); i++) {
                vec2 pos = gl_in[i].gl_Position.xy;

                gl_Position = vec4(pos +
                                   vec2(0.0, vert_offset),
                                   0.0, 1.0);
                fs_color = gs_color[i];
                EmitVertex();

                gl_Position = vec4(pos +
                                   vec2(-vert_offset),
                                   0.0, 1.0);
                fs_color = gs_color[i];
                EmitVertex();

                gl_Position = vec4(pos +
                                   vec2(vert_offset, -vert_offset),
                                   0.0, 1.0);
                fs_color = gs_color[i];
                EmitVertex();

                EndPrimitive();
        }
}
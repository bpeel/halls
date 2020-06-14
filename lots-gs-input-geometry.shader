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
                vec4 pos = gl_in[i].gl_Position;

                gl_Position = vec4(pos.xy +
                                   vec2(0.0, vert_offset),
                                   pos.zw);
                fs_color = gs_color[i];
                EmitVertex();

                gl_Position = vec4(pos.xy +
                                   vec2(-vert_offset),
                                   pos.zw);
                fs_color = gs_color[i];
                EmitVertex();

                gl_Position = vec4(pos.xy +
                                   vec2(vert_offset, -vert_offset),
                                   pos.zw);
                fs_color = gs_color[i];
                EmitVertex();

                EndPrimitive();
        }
}

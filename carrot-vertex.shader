#version 300 es

layout(location = 0) in vec2 vertex;

out vec2 fragTexCoord;

void main()
{
    gl_Position = vec4(vertex, 0.0, 1.0);
    fragTexCoord = vertex * vec2(0.5, -0.5) + 0.5;
}

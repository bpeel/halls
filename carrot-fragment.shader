#version 300 es

precision mediump float;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 fragTexCoord;

out vec4 outColour;

void main()
{
    vec3 ycbcr = vec3(texture(texture0, fragTexCoord).r,
                      texture(texture1, fragTexCoord).r,
                      texture(texture2, fragTexCoord).r);

    ycbcr += vec3(-0.0625, -0.5, -0.5);
    outColour = vec4(dot(ycbcr, vec3(1.164,  0.0  ,  1.596)),
                     dot(ycbcr, vec3(1.164, -0.391, -0.813)),
                     dot(ycbcr, vec3(1.164,  2.018,  0.0  )),
                     1.0);
}

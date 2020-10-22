precision mediump float;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

varying vec2 tex_coord;

void main()
{
        vec2 flipped_coord = vec2(tex_coord.x, 1.0 - tex_coord.y);
        vec3 ycbcr = vec3(texture2D(texture0, flipped_coord).a,
                          texture2D(texture1, flipped_coord).a,
                          texture2D(texture2, flipped_coord).a);

        ycbcr += vec3(-0.0625, -0.5, -0.5);
        gl_FragColor = vec4(dot(ycbcr, vec3(1.164,  0.0  ,  1.596)),
                            dot(ycbcr, vec3(1.164, -0.391, -0.813)),
                            dot(ycbcr, vec3(1.164,  2.018,  0.0  )),
                            1.0);
}

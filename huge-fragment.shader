#ifdef GL_ES
precision mediump float;
#endif

varying vec2 tex_coord;

#define TEXTURES_PER_DRAW 4

uniform sampler2D tex[TEXTURES_PER_DRAW];

void
main()
{
        vec4 sum;

        sum = texture2D(tex[0], tex_coord);
        sum += texture2D(tex[1], tex_coord);
        sum += texture2D(tex[2], tex_coord);
        sum += texture2D(tex[3], tex_coord);

        gl_FragColor = sum / float(TEXTURES_PER_DRAW);
}

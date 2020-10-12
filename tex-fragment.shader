#ifdef GL_ES
precision mediump float;
#endif

varying vec2 tex_coord;

uniform sampler2D tex;

void
main()
{
        gl_FragColor = texture2D(tex, tex_coord);
}

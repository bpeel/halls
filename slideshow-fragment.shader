varying mediump vec2 tex_coord;

uniform mediump sampler2D tex;

void
main()
{
        gl_FragColor = texture2D(tex, tex_coord);
}

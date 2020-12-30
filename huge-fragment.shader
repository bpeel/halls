#ifdef GL_ES
precision mediump float;
#endif

varying vec2 tex_coord;

#define TEXTURES_PER_DRAW 4
#define GRID_SIZE 5

uniform sampler2D tex[TEXTURES_PER_DRAW];

void
main()
{
        vec4 sum = vec4(0.0);

        for (int x = -(GRID_SIZE / 2); x <= GRID_SIZE / 2; x++) {
                for (int y = -(GRID_SIZE / 2); y <= GRID_SIZE / 2; y++) {
                        sum += texture2D(tex[0], tex_coord + vec2(x, y) * 0.1);
                        sum += texture2D(tex[1], tex_coord + vec2(x, y) * 0.1);
                        sum += texture2D(tex[2], tex_coord + vec2(x, y) * 0.1);
                        sum += texture2D(tex[3], tex_coord + vec2(x, y) * 0.1);
                }
        }

        gl_FragColor = sum / float(TEXTURES_PER_DRAW * GRID_SIZE * GRID_SIZE);
}

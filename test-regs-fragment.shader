#version 310 es

precision mediump float;

layout(location = 0) out vec4 color_out;
in float x_coord;

uniform int a, b;

uniform sampler2D tex;

#define N_SAMPS 32

void
main()
{
        vec2 samps[N_SAMPS];

        /* Sample 16 different coordinates. These all end up actually
         * sampling the same location.
         */
        for (int i = 0; i < N_SAMPS; i++) {
                samps[i] = texture2D(tex,
                                  vec2(x_coord * 5.0 +
                                       float(i), 0.5)).xy;
        }

        float sum = 0.0;

        for (int i = 0; i < N_SAMPS; i++)
                sum += samps[i].x + samps[i].y;

        /* This should end up as 0.5 for all the fragments */
        color_out = vec4(sum / float(N_SAMPS) / 2.0 -
                         fract(x_coord * 5.0) + 0.5);
}

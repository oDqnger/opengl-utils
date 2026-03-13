#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cglm/cglm.h>

#define SEED 50

static double ease_function(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static void gradient(int ix, int iy, int seed, vec2 out) {
    // Replicate Python's seeded random: seed the RNG with a hash of ix, iy, seed
    srand((unsigned int)(ix * 374761393 + iy * 668265263 + seed));
    double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
    out[0] = (float)cos(angle);
    out[1] = (float)sin(angle);
}

double perlin(vec2 coords, double cell_size, int seed) {
    int cell_x = (int)floor(coords[0] / cell_size);
    int cell_y = (int)floor(coords[1] / cell_size);

    double local_x = (coords[0] / cell_size) - cell_x;
    double local_y = (coords[1] / cell_size) - cell_y;

    int corners[4][2] = {
        { cell_x,     cell_y     },
        { cell_x,     cell_y + 1 },
        { cell_x + 1, cell_y     },
        { cell_x + 1, cell_y + 1 }
    };

    float dots[4];

    for (int i = 0; i < 4; i++) {
        int cx = corners[i][0];
        int cy = corners[i][1];

        vec2 grad;
        gradient(cx, cy, seed, grad);

        float dx = (float)(local_x - (cx - cell_x));
        float dy = (float)(local_y - (cy - cell_y));

        vec2 offset = { dx, dy };
        dots[i] = glm_vec2_dot(grad, offset);
    }

    double fx = ease_function(local_x);
    double fy = ease_function(local_y);

    double l = dots[0] + fx * (dots[2] - dots[0]);
    double u = dots[1] + fx * (dots[3] - dots[1]);

    double noise = l + fy * (u - l);

    return fabs(((noise + 1.0) / 2.0)*10);
}

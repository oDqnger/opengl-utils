#include "bullet.h"
#include "stb_ds.h"
#include <stdlib.h>
#include <stdio.h>
#include <cglm/cglm.h>

Bullet* create_bullet(vec3 dir, vec3 cameraPos) {
    Bullet* bullet = malloc(sizeof(Bullet));
    bullet->dir[0] = dir[0];
    bullet->dir[1] = dir[1];
    bullet->dir[2] = dir[2];
    bullet->currentPos[0] = cameraPos[0];
    bullet->currentPos[1] = cameraPos[1];
    bullet->currentPos[2] = cameraPos[2];

    return bullet;
}


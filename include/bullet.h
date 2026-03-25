#ifndef BULLET_H
#define BULLET_H

#include <cglm/cglm.h>

typedef struct {
    vec3 currentPos;
    vec3 dir;
} Bullet;

Bullet* create_bullet(vec3 dir, vec3 cameraPos);

#endif
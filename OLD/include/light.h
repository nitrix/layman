#ifndef LEARNGL_LIGHT_H
#define LEARNGL_LIGHT_H

#include "vector.h"
#include "toolkit.h"

struct light {
    struct vector3f position;

    struct vector3f ambient;
    struct vector3f diffuse;
    struct vector3f specular;
};

struct light *light_create(void);
void light_destroy(struct light *light);

void light_move(struct light *light, float dx, float dy, float dz);

#endif

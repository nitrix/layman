#ifndef LEARNGL_MATERIAL_H
#define LEARNGL_MATERIAL_H

#include "vector.h"

struct material {
    struct vector3f ambient;
    struct vector3f diffuse;
    struct vector3f specular;
    float shininess;
};

struct material *material_create(void);
void material_destroy(struct material *material);

#endif

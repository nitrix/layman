#ifndef LEARNGL_ENTITY_H
#define LEARNGL_ENTITY_H

#include "vector.h"

struct entity {
    struct model *model;
    struct shader *shader;
    struct texture *texture;

    struct vector3f position;
    struct vector3f rotation;
    float scale;
};

struct entity *entity_create(void);
void entity_destroy(struct entity *entity);

void entity_move(struct entity *entity, float dx, float dy, float dz);
void entity_rotate(struct entity *entity, float dx, float dy, float dz);

#endif
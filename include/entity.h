#ifndef LEARNGL_ENTITY_H
#define LEARNGL_ENTITY_H

#include "vector.h"
#include "matrix.h"
#include "direction.h"
#include "renderer.h"

struct entity {
    struct model *model;
    struct shader *shader;
    struct texture *texture;

    struct vector3f position;
    struct vector3f rotation;
    float scale;

    float move_speed;
    float turn_speed;

    struct matrix4f model_matrix;

    float shine_damper;
    float reflectivity;
};

struct entity *entity_create(void);
void entity_destroy(struct entity *entity);

void entity_ugly_move(struct entity *entity, direction_mask direction, struct renderer *renderer);
void entity_move(struct entity *entity, float dx, float dy, float dz);
void entity_set_position(struct entity *entity, float x, float y, float z);
void entity_rotate(struct entity *entity, float dx, float dy, float dz);

void entity_absolute_move(struct entity *entity, float dx, float dy, float dz);
void entity_relative_move(struct entity *entity, direction_mask direction, float move_amount, float turn_amount);

struct matrix4f *entity_model_matrix(struct entity *entity);

#endif
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

    struct matrix4f model_matrix;
    bool model_matrix_up_to_date;

    float shine_damper;
    float reflectivity;
};

struct entity *entity_create(void);
void entity_destroy(struct entity *entity);

void entity_set_position(struct entity *entity, float x, float y, float z);
void entity_rotate(struct entity *entity, float dx, float dy, float dz);
void entity_absolute_move(struct entity *entity, float dx, float dy, float dz);
void entity_relative_move(struct entity *entity, direction_mask direction, float move_amount, float turn_amount);

const struct matrix4f *entity_model_matrix(struct entity *entity);

#endif
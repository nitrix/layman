#include "entity.h"
#include "toolkit.h"
#include "renderer.h"
#include "math.h"

#define ENTITY_MOVE_SPEED 3000
#define ENTITY_TURN_SPEED 500

void entity_update_model_matrix(struct entity *entity);

struct entity *entity_create(void) {
    struct entity *entity = malloc(sizeof *entity);

    if (!entity) {
        return NULL;
    }

    // Position
    entity->position.x = 0;
    entity->position.y = 0;
    entity->position.z = 0;

    // Rotation
    entity->rotation.x = 0;
    entity->rotation.y = 0;
    entity->rotation.z = 0;

    // Scale
    entity->scale = 1.0f;

    // Motion
    entity->move_speed = 0;
    entity->turn_speed = 0;

    // Computed transformation matrix
    entity->model_matrix = matrix_identity();

    return entity;
}

void entity_destroy(struct entity *entity) {
    free(entity);
}

void entity_relative_move(struct entity *entity, direction_mask direction, float move_amount, float turn_amount) {
    // Rotation
    if (TK_MASK_IS_SET(direction, DIRECTION_LEFT)) {
        turn_amount = -turn_amount;
    }
    else if (TK_MASK_IS_SET(direction, DIRECTION_RIGHT)) {
        turn_amount = turn_amount;
    }
    else {
        turn_amount = 0;
    }

    entity_rotate(entity, 0, turn_amount, 0);

    // Translation
    if (TK_MASK_IS_SET(direction, DIRECTION_FORWARD)) {
        move_amount = move_amount;
    }
    else if (TK_MASK_IS_SET(direction, DIRECTION_BACKWARD)) {
        move_amount = -move_amount;
    }
    else {
        move_amount = 0;
    }

    float delta_x = move_amount * sinf(entity->rotation.y * -1);
    float delta_z = move_amount * cosf(entity->rotation.y * -1);

    // TODO: We're doing model matrix updates twice
    entity_absolute_move(entity, delta_x, 0, delta_z);
}

void entity_absolute_move(struct entity *entity, float dx, float dy, float dz) {
    entity->position.x += dx;
    entity->position.y += dy;
    entity->position.z += dz;

    entity_update_model_matrix(entity);
}

void entity_set_position(struct entity *entity, float x, float y, float z) {
    entity->position.x = x;
    entity->position.y = y;
    entity->position.z = z;

    entity_update_model_matrix(entity);
}

void entity_rotate(struct entity *entity, float dx, float dy, float dz) {
    entity->rotation.x += dx;
    entity->rotation.y += dy;
    entity->rotation.z += dz;

    entity_update_model_matrix(entity);
}

struct matrix4f *entity_model_matrix(struct entity *entity) {
    return &entity->model_matrix;
}

void entity_update_model_matrix(struct entity *entity) {
    entity->model_matrix = matrix_identity();

    // Important: the ordering matters!
    // It must be the scaling FIRST, and THEN rotation, and THEN the translation.

    matrix_scale(&entity->model_matrix, entity->scale, entity->scale, entity->scale);

    matrix_rotate_x(&entity->model_matrix, entity->rotation.x);
    matrix_rotate_y(&entity->model_matrix, entity->rotation.y);
    matrix_rotate_z(&entity->model_matrix, entity->rotation.z);

    matrix_translate(&entity->model_matrix, &entity->position);
}
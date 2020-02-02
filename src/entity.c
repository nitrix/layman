#include "entity.h"
#include "toolkit.h"
#include "renderer.h"
#include "math.h"

#define ENTITY_MOVE_SPEED 300
#define ENTITY_TURN_SPEED 300

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

// TODO: Proof of concept, fix this ugly shit.
void entity_ugly_move(struct entity *entity, direction_mask direction, struct renderer *renderer) {
    // Rotation
    if (TK_MASK_IS_SET(direction, DIRECTION_LEFT)) {
        entity->turn_speed = ENTITY_TURN_SPEED;
    }
    else if (TK_MASK_IS_SET(direction, DIRECTION_RIGHT)) {
        entity->turn_speed = -ENTITY_TURN_SPEED;
    }
    else {
        entity->turn_speed = 0;
    }

    entity_rotate(entity, 0, entity->turn_speed * (float) renderer_frame_time_delta(renderer), 0);

    // Translation
    if (TK_MASK_IS_SET(direction, DIRECTION_FORWARD)) {
        entity->move_speed = ENTITY_MOVE_SPEED;
    }
    else if (TK_MASK_IS_SET(direction, DIRECTION_BACKWARD)) {
        entity->move_speed = -ENTITY_MOVE_SPEED;
    }
    else {
        entity->move_speed = 0;
    }

    float distance = entity->move_speed * (float) renderer_frame_time_delta(renderer);
    float delta_x = distance * sinf(entity->rotation.y) * -1;
    float delta_z = distance * cosf(entity->rotation.y);

    // TODO: Doing model matrix updates twice
    entity_move(entity, delta_x, 0, delta_z);
}

void entity_move(struct entity *entity, float dx, float dy, float dz) {
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

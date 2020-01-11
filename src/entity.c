#include "entity.h"
#include "toolkit.h"

struct entity *entity_create(void) {
    struct entity *entity = malloc(sizeof *entity);

    if (!entity) {
        return NULL;
    }

    entity->position = (struct vector3f) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    entity->rotation = (struct vector3f) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    entity->scale = 1.0f;

    return entity;
}

void entity_destroy(struct entity *entity) {
    free(entity);
}

void entity_move(struct entity *entity, float dx, float dy, float dz) {
    entity->position.x += dx;
    entity->position.y += dy;
    entity->position.z += dz;
}

void entity_rotate(struct entity *entity, float dx, float dy, float dz) {
    entity->rotation.x += dx;
    entity->rotation.y += dy;
    entity->rotation.z += dz;
}
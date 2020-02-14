#include "light.h"

struct light *light_create(void) {
    struct light *light = malloc(sizeof *light);

    if (!light) {
        return NULL;
    }

    // TODO
    light->position = (struct vector3f) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    light->color = (struct vector3f) { .x = 1.0f, .y = 1.0f, .z = 1.0f };

    return light;
}

void light_destroy(struct light *light) {
    free(light);
}

void light_move(struct light *light, float dx, float dy, float dz) {
    light->position.x += dx;
    light->position.y += dy;
    light->position.z += dz;
}

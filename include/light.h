#ifndef LEARNGL_LIGHT_H
#define LEARNGL_LIGHT_H

#include "vector.h"

struct light;

struct light *light_create(void);
void light_destroy(struct light *light);

void light_move(struct light *light, float dx, float dy, float dz);

const struct vector3f *light_get_position(const struct light *light);
const struct vector3f *light_get_color(const struct light *light);

#endif

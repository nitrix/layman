#ifndef LEARNGL_LIGHT_H
#define LEARNGL_LIGHT_H

#include "vector.h"

struct light;

struct light *light_create(void);
void light_destroy(struct light *light);

void light_move(struct light *light, float dx, float dy, float dz);

struct vector3f *light_get_position(struct light *light);
struct vector3f *light_get_color(struct light *light);

#endif
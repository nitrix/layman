#include "client.h"

// FIXME: Lights defaults are currently hard-coded. We need nicer functions to configure them.

void light_init(struct light *light, enum light_type type) {
	light->type = type;
	glm_vec3_zero(light->position);
	VEC3_ASSIGN(light->direction, 0, 0, -1);
	light->range = 10; // FIXME?
	VEC3_ASSIGN(light->color, 1, 1, 1); // FIXME?
	light->intensity = 1;
	light->innerConeCos = 1; // FIXME?
	light->outerConeCos = 1; // FIXME?
}

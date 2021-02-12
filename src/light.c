#include "layman.h"

struct layman_light *layman_light_create(enum layman_light_type type) {
	struct layman_light *light = malloc(sizeof *light);
	if (!light) {
		return NULL;
	}

	light->type = type;
	glm_vec3_zero(light->position);
	VEC3_ASSIGN(light->direction, 0, 0, -1);
	light->range = 10; // FIXME?
	VEC3_ASSIGN(light->color, 1, 1, 1);
	light->intensity = 1;
	light->innerConeCos = 1; // FIXME?
	light->outerConeCos = 1; // FIXME?

	return light;
}

void layman_light_destroy(struct layman_light *light) {
	free(light);
}

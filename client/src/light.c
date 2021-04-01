#include "cglm/cglm.h"
#include "light.h"
#include "utils.h"

struct light *light_create(enum light_type type) {
	struct light *light = malloc(sizeof *light);
	if (!light) {
		return NULL;
	}

	light->type = type;
	glm_vec3_zero(light->position);
	VEC3_ASSIGN(light->direction, 0, 0, -1);
	light->range = 10; // FIXME?
	VEC3_ASSIGN(light->color, 1, 0, 0);
	light->intensity = 1;
	light->innerConeCos = 1; // FIXME?
	light->outerConeCos = 1; // FIXME?

	return light;
}

void light_destroy(struct light *light) {
	free(light);
}

#include "client.h"

void light_init(struct light *light, enum light_type type) {
	light->type = type;
	glm_vec3_zero(light->position);
	glm_vec3_copy((vec3) { 0, 0, -1}, light->direction);
	light->range = 5;
	glm_vec3_copy((vec3) { 1, 1, 1}, light->color);
	light->intensity = 1;
	light->innerConeCos = 1;
	light->outerConeCos = 1;
}

#include "cglm/cglm.h"
#include "entity.h"
#include <stdlib.h>

static uint32_t next_entity_id = 123456;

struct entity *entity_create(void) {
	struct entity *entity = malloc(sizeof *entity);
	if (!entity) {
		return NULL;
	}

	entity->model = NULL;
	glm_vec3_zero(entity->translation);
	glm_quat_identity(entity->rotation);
	entity->scale = 1;
	entity->id = next_entity_id++;

	return entity;
}

void entity_id_as_color(uint32_t id, vec4 color) {
	int r = (id & 0x000000FF) >> 0;
	int g = (id & 0x0000FF00) >> 8;
	int b = (id & 0x00FF0000) >> 16;
	int a = (id & 0xFF000000) >> 24;

	color[0] = r / 255.0f;
	color[1] = g / 255.0f;
	color[2] = b / 255.0f;
	color[3] = a / 255.0f;
}

uint32_t entity_color_as_id(vec4 color) {
	int r = color[0] * 255.0f;
	int g = color[1] * 255.0f;
	int b = color[2] * 255.0f;
	int a = color[3] * 255.0f;

	return r + (g * 256) + (b * 256 * 256) + (a * 256 * 256 * 256);
}

struct entity *entity_create_from_model(const struct model *model) {
	struct entity *entity = entity_create();
	if (!entity) {
		return NULL;
	}

	entity->model = model;

	return entity;
}

void entity_destroy(struct entity *entity) {
	free(entity);
}

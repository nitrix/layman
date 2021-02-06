#include "layman.h"

struct layman_entity *layman_entity_create(void) {
	struct layman_entity *entity = malloc(sizeof *entity);
	if (!entity) {
		return NULL;
	}

	entity->model = NULL;
	glm_vec3_zero(entity->position);

	return entity;
}

struct layman_entity *layman_entity_create_from_model(const struct layman_model *model) {
	struct layman_entity *entity = layman_entity_create();
	if (!entity) {
		return NULL;
	}

	entity->model = model;

	return entity;
}

void layman_entity_destroy(struct layman_entity *entity) {
	free(entity);
}

#include "cglm/cglm.h"
#include "entity.h"
#include <stdlib.h>

struct entity *entity_create(void) {
	struct entity *entity = malloc(sizeof *entity);
	if (!entity) {
		return NULL;
	}

	entity->model = NULL;
	glm_vec3_zero(entity->position);
	glm_vec3_zero(entity->rotation);
	entity->scale = 1;

	return entity;
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

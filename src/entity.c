#include "layman.h"
#include <stdlib.h>

struct layman_entity {
	const struct layman_model *model;
	struct layman_vector_3f position;
};

struct layman_entity *layman_entity_create_from_model(const struct layman_model *model) {
	struct layman_entity *entity = malloc(sizeof *entity);
	if (!entity) {
		return NULL;
	}

	entity->model = model;
	layman_vector_3f_init(&entity->position);

	return entity;
}

void layman_entity_destroy(struct layman_entity *entity) {
	free(entity);
}

void layman_entity_render(const struct layman_entity *entity) {
	layman_model_render(entity->model);
}
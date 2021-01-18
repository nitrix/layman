#include "layman.h"
#include <stdlib.h>

struct layman_entity *layman_entity_create(void) {
	struct layman_entity *entity = malloc(sizeof *entity);
	if (!entity) {
		return NULL;
	}

	entity->model = NULL;
	layman_vector_3f_init(&entity->position);

	return entity;
}

void layman_entity_destroy(struct layman_entity *entity) {
	free(entity);
}
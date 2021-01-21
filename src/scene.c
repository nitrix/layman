#include "layman.h"
#include <stdbool.h>
#include <stdlib.h>

#define ENTITIES_CAPACITY_STEP 16

// Scenes can only grow in size. The idea being that if there was ever at some point X entities,
// it's equally likely in the future to have just as many entities again.

struct layman_scene {
	const struct layman_entity **entities;
	size_t entity_count;
	size_t entity_capacity;
};

struct layman_scene *layman_scene_create(void) {
	struct layman_scene *scene = malloc(sizeof *scene);
	if (!scene) {
		return NULL;
	}

	scene->entities = NULL;
	scene->entity_count = 0;
	scene->entity_capacity = 0;

	return scene;
}

void layman_scene_destroy(struct layman_scene *scene) {
	free(scene);
}

bool layman_scene_add_entity(struct layman_scene *scene, const struct layman_entity *entity) {
	bool full = scene->entity_count == scene->entity_capacity;

	if (full) {
		size_t new_capacity = scene->entity_capacity + ENTITIES_CAPACITY_STEP;

		const struct layman_entity **new_entities = realloc(scene->entities, new_capacity * sizeof *new_entities);
		if (!new_entities) {
			return false;
		}

		scene->entities = new_entities;
		scene->entity_capacity = new_capacity;
	}

	scene->entities[scene->entity_count] = entity;
	scene->entity_count++;

	return true;
}

void layman_scene_render_entities(const struct layman_scene *scene) {
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct layman_entity *entity = scene->entities[i];
		layman_entity_render(entity);
	}
}
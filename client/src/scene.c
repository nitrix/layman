#include "scene.h"
#include <stdlib.h>

#define ENTITIES_CAPACITY_STEP 16

// Scenes can only grow in size. The idea being that if there was ever at some point X entities,
// it's equally likely in the future to have just as many entities again.

struct scene *scene_create(void) {
	struct scene *scene = malloc(sizeof *scene);
	if (!scene) {
		return NULL;
	}

	scene->entities = NULL;
	scene->entity_count = 0;
	scene->entity_capacity = 0;

	scene->lights = NULL;
	scene->lights_count = 0;

	return scene;
}

void scene_destroy(struct scene *scene) {
	free(scene->entities);
	free(scene->lights);
	free(scene);
}

bool scene_add_entity(struct scene *scene, const struct entity *entity) {
	bool full = scene->entity_count == scene->entity_capacity;

	if (full) {
		size_t new_capacity = scene->entity_capacity + ENTITIES_CAPACITY_STEP;

		const struct entity **new_entities = realloc(scene->entities, new_capacity * sizeof *new_entities);
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

bool scene_add_light(struct scene *scene, const struct light *light) {
	const struct light **new_lights = realloc(scene->lights, (scene->lights_count + 1) * sizeof *scene->lights);
	if (!new_lights) {
		return false;
	}

	new_lights[scene->lights_count] = light;
	scene->lights = new_lights;
	scene->lights_count++;

	return true;
}

void scene_assign_environment(struct scene *scene, const struct environment *environment) {
	scene->environment = environment;
}
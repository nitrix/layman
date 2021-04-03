#ifndef SCENE_H
#define SCENE_H

struct scene {
	struct entity **entities;
	size_t entity_count;
	size_t entity_capacity;

	const struct light **lights;
	size_t lights_count;

	const struct environment *environment;
};

struct scene *scene_create(void);
void scene_destroy(struct scene *scene);
bool scene_add_entity(struct scene *scene, struct entity *entity);
bool scene_add_light(struct scene *scene, const struct light *light);
void scene_assign_environment(struct scene *scene, const struct environment *environment);

#endif

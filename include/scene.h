#ifndef SCENE_H
#define SCENE_H

struct scene {
	struct entity **entities;
	size_t entity_count;
	size_t entity_capacity;

	const struct light **lights;
	size_t lights_count;

	struct environment *environment;
};

void scene_init(struct scene *scene);
void scene_fini(struct scene *scene);
bool scene_add_entity(struct scene *scene, struct entity *entity);
bool scene_add_light(struct scene *scene, const struct light *light);

#endif

#ifndef LAYMAN_PRIVATE_SCENE_H
#define LAYMAN_PRIVATE_SCENE_H

struct layman_scene {
	const struct layman_entity **entities;
	size_t entity_count;
	size_t entity_capacity;

	const struct layman_light **lights;
	size_t lights_count;

	const struct layman_environment *environment;
};

#endif

#ifndef SCENE_H
#define SCENE_H

#include "entity.h"
#include "light.h"
#include <stdbool.h>

struct scene {
	struct entity **entities;
	size_t entity_count;
	size_t entity_capacity;

	const struct light **lights;
	size_t lights_count;

	const struct environment *environment;
};

/**
 * @brief Creates a scene.
 *
 * Scenes are efficient data structures that keeps track of all the entities that needs to be rendered.
 *
 * @remark Scenes are manually managed and must be destroyed using scene_destroy().
 *
 * @return A pointer to a scene or `NULL` on failure.
 */
struct scene *scene_create(void);

/**
 * @brief Destroys a scene.
 *
 * @param[in] scene A pointer to the scene to destroy.
 */
void scene_destroy(struct scene *scene);

/**
 * @brief Add an entity to a scene.
 *
 * @param[in] scene A pointer to the scene.
 * @param[in] entity A pointer to the entity.
 *
 * @par Performance
 * The entities are sorted by their models to minimize the switching overhead during rendering.
 *
 * @par Ownership/lifetime
 * - The user **maintains** ownership over the entity.
 * - They must ensure the entity lifetime is larger than its presence within the scene.
 *
 * @return Returns `true` on success or `false` otherwise.
 */
bool scene_add_entity(struct scene *scene, struct entity *entity);

// TODO: Documentation.
bool scene_add_light(struct scene *scene, const struct light *light);

// TODO: Documentation.
void scene_assign_environment(struct scene *scene, const struct environment *environment);

#endif

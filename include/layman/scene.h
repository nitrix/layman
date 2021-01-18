#ifndef LAYMAN_SCENE_H
#define LAYMAN_SCENE_H

#include <stdbool.h>

struct layman_scene {
    const struct layman_entity **entities;
    size_t entity_count;
    size_t entity_capacity;
};

/**
 * @brief Creates a scene.
 * 
 * Scenes are efficient data structures that keeps track of all the entities that needs to be rendered.
 * 
 * @remark Scenes are manually managed and must be destroyed using layman_scene_destroy().
 * 
 * @return A pointer to a scene or `NULL` on failure.
 */
struct layman_scene *layman_scene_create(void);

/**
 * @brief Destroys a scene.
 * 
 * @param[in] scene A pointer to the scene to destroy.
 */
void layman_scene_destroy(struct layman_scene *scene);

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
bool layman_scene_add_entity(struct layman_scene *scene, const struct layman_entity *entity);

#endif
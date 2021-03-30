#ifndef ENTITY_H
#define ENTITY_H

#include "cglm/cglm.h"
#include "model.h"

struct entity {
	const struct model *model;
	vec3 translation;
	vec3 rotation;
	float scale;
};

/**
 * @brief Creates an entity.
 *
 * @remark Entities are manually managed and must be destroyed with entity_destroy().
 *
 * @return A pointer to an entity on success or NULL otherwise.
 **/
struct entity *entity_create(void);

/**
 * @brief Creates an entity from a model.
 *
 * @param[in] model A pointer to a model.
 *
 * @remark The model must remain alive for the entire lifetime of the entity.
 * @remark Entities are manually managed and must be destroyed with entity_destroy().
 *
 * @return A pointer to an entity on success or NULL otherwise.
 **/
struct entity *entity_create_from_model(const struct model *model);

/**
 * @brief Destroys an entity.
 *
 * @param[in] entity A pointer to the entity to destroy.
 */
void entity_destroy(struct entity *entity);

#endif

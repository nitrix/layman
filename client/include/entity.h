#ifndef ENTITY_H
#define ENTITY_H

#include "cglm/cglm.h"
#include "model.h"

struct entity {
	uint32_t id;
	const struct model *model;
	vec3 translation;
	versor rotation;
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

void entity_id_as_color(uint32_t id, vec4 color);
uint32_t entity_color_as_id(vec4 color);

#endif

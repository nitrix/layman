#ifndef LAYMAN_ENTITY_H
#define LAYMAN_ENTITY_H

#include "vector.h"

struct layman_entity {
	struct layman_model *model;
	struct layman_vector_3f position;
};

/**
 * @brief Creates an entity.
 * 
 * @remark Entities are manually managed and must be destroyed with layman_entity_destroy().
 * 
 * @return A pointer to an entity on success or NULL otherwise.
 **/
struct layman_entity *layman_entity_create(void);

/**
 * @brief Destroys an entity.
 * 
 * @param[in] entity A pointer to the entity to destroy.
 */
void layman_entity_destroy(struct layman_entity *entity);

#endif
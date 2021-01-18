#ifndef LAYMAN_ENTITY_H
#define LAYMAN_ENTITY_H

#include "model.h"
#include "vector.h"

/**
 * @brief Creates an entity from a model.
 * 
 * @param[in] model A pointer to a model.
 * The model must remain alive for the entire lifetime of the entity.
 * 
 * @remark Entities are manually managed and must be destroyed with layman_entity_destroy().
 * 
 * @return A pointer to an entity on success or NULL otherwise.
 **/
struct layman_entity *layman_entity_create_from_model(const struct layman_model *model);

/**
 * @brief Destroys an entity.
 * 
 * @param[in] entity A pointer to the entity to destroy.
 */
void layman_entity_destroy(struct layman_entity *entity);

// TODO
void layman_entity_render(const struct layman_entity *entity);

#endif
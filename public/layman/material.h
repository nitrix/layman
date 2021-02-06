#ifndef LAYMAN_MATERIAL_H
#define LAYMAN_MATERIAL_H

#include "texture.h"

/**
 * @brief Creates a default material.
 *
 * @remark Materials are manually managed and must later detroyed with layman_material_destroy().
 *
 * @returns A pointer to the material.
 */
struct layman_material *layman_material_create(void);

/**
 * @brief Destroys a material.
 *
 * @remark The associated textures are freed automatically as well.
 */
void layman_material_destroy(struct layman_material *material);

#endif

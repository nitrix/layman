#ifndef LAYMAN_MATERIAL_H
#define LAYMAN_MATERIAL_H

#include "texture.h"
#include "vector.h"

struct layman_material *layman_material_create(void);
void layman_material_destroy(struct layman_material *material);

#endif
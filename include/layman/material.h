#ifndef LAYMAN_MATERIAL_H
#define LAYMAN_MATERIAL_H

#include "texture.h"
#include "vector.h"

struct layman_material {
	struct layman_vector_3f base_color_factor;
	struct layman_texture *base_color_texture;
	struct layman_texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;
	struct layman_texture *normal_texture;
	struct layman_texture *occlusion_texture;
	struct layman_texture *emissive_texture;
	struct layman_vector_3f emissive_factor;
};

struct layman_material *layman_material_create(void);
void layman_material_destroy(struct layman_material *material);

#endif
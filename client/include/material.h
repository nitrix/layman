#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/cglm.h"
#include "texture.h"

struct material {
	vec4 base_color_factor;
	struct texture *base_color_texture;
	struct texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;
	struct texture *normal_texture;
	float normal_scale;
	struct texture *occlusion_texture;
	float occlusion_strength;
	struct texture *emissive_texture;
	vec3 emissive_factor;
};

void material_switch(const struct material *material);

/**
 * @brief Creates a default material.
 *
 * @remark Materials are manually managed and must later detroyed with layman_material_destroy().
 *
 * @returns A pointer to the material.
 */
struct material *material_create(void);

/**
 * @brief Destroys a material.
 *
 * @remark The associated textures are freed automatically as well.
 */
void material_destroy(struct material *material);

#endif

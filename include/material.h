#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/cglm.h"

struct material {
	char *name;

	// Mettalic/roughness.
	vec4 base_color_factor;
	struct texture *base_color_texture;
	struct texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;

	// Other things.
	struct texture *normal_texture;
	float normal_scale;
	struct texture *occlusion_texture;
	float occlusion_strength;
	struct texture *emissive_texture;
	vec3 emissive_factor;

	bool double_sided;
};

void material_init(struct material *material);
void material_fini(struct material *material);
void material_switch(const struct material *material);

#endif

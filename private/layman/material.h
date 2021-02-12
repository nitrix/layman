#ifndef LAYMAN_PRIVATE_MATERIAL_H
#define LAYMAN_PRIVATE_MATERIAL_H

struct layman_material {
	vec4 base_color_factor;
	struct layman_texture *base_color_texture;
	struct layman_texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;
	struct layman_texture *normal_texture;
	float normal_scale;
	struct layman_texture *occlusion_texture;
	float occlusion_strength;
	struct layman_texture *emissive_texture;
	vec3 emissive_factor;
};

void layman_material_switch(const struct layman_material *material);

#endif

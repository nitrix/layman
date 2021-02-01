#ifndef LAYMAN_PRIVATE_MATERIAL_H
#define LAYMAN_PRIVATE_MATERIAL_H

struct layman_material {
	struct layman_vector_4f base_color_factor;
	struct layman_texture *base_color_texture;
	struct layman_texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;
	struct layman_texture *normal_texture;
	struct layman_texture *occlusion_texture;
	float occlusion_strength;
	struct layman_texture *emissive_texture;
	struct layman_vector_3f emissive_factor;
};

#endif

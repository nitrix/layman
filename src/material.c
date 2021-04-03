#include "material.h"
#include "texture.h"

void material_init(struct material *material) {
	// Mettalic/roughness.
	glm_vec4_one(material->base_color_factor);
	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->metallic_factor = 1;
	material->roughness_factor = 1;

	// Other things.
	material->normal_texture = NULL;
	material->normal_scale = 1;
	material->occlusion_texture = NULL;
	material->occlusion_strength = 1;
	material->emissive_texture = NULL;
	glm_vec3_zero(material->emissive_factor);
}

void material_fini(struct material *material) {
	if (material->base_color_texture) {
		texture_fini(material->base_color_texture);
	}

	if (material->metallic_roughness_texture) {
		texture_fini(material->metallic_roughness_texture);
	}

	if (material->normal_texture) {
		texture_fini(material->normal_texture);
	}

	if (material->occlusion_texture) {
		texture_fini(material->occlusion_texture);
	}

	if (material->emissive_texture) {
		texture_fini(material->emissive_texture);
	}
}

void material_switch(const struct material *material) {
	if (material->base_color_texture) {
		texture_switch(material->base_color_texture);
	}

	if (material->normal_texture) {
		texture_switch(material->normal_texture);
	}

	if (material->metallic_roughness_texture) {
		texture_switch(material->metallic_roughness_texture);
	}

	if (material->occlusion_texture) {
		texture_switch(material->occlusion_texture);
	}

	if (material->emissive_texture) {
		texture_switch(material->emissive_texture);
	}
}

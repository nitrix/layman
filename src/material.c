#include "layman.h"

thread_local const struct layman_material *current_material;

struct layman_material *layman_material_create(void) {
	struct layman_material *material = malloc(sizeof *material);
	if (!material) {
		return NULL;
	}

	glm_vec4_one(material->base_color_factor);
	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->metallic_factor = 0; // FIXME?
	material->roughness_factor = 0; // FIXME?
	material->normal_texture = NULL;
	material->normal_scale = 1;
	material->occlusion_texture = NULL;
	material->occlusion_strength = 1;
	material->emissive_texture = NULL;
	glm_vec3_one(material->emissive_factor);

	return material;
}

void layman_material_destroy(struct layman_material *material) {
	layman_texture_destroy(material->base_color_texture);
	layman_texture_destroy(material->metallic_roughness_texture);
	layman_texture_destroy(material->normal_texture);
	layman_texture_destroy(material->occlusion_texture);
	layman_texture_destroy(material->emissive_texture);

	free(material);
}

void layman_material_switch(const struct layman_material *material) {
	if (current_material == material) {
		return;
	} else {
		current_material = material;
	}

	if (material) {
		layman_texture_switch(material->base_color_texture);
		layman_texture_switch(material->normal_texture);
		layman_texture_switch(material->metallic_roughness_texture);
		layman_texture_switch(material->occlusion_texture);
		layman_texture_switch(material->emissive_texture);
	} else {
		layman_texture_switch(NULL);
	}
}

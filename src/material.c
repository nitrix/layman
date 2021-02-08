#include "layman.h"

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

void layman_material_switch(const struct layman_material *new) {
	thread_local static const struct layman_material *current;

	if (new == current) {
		return;
	}

	current = new;

	if (new) {
		layman_texture_switch(new->base_color_texture);
		layman_texture_switch(new->normal_texture);
		layman_texture_switch(new->metallic_roughness_texture);
		layman_texture_switch(new->occlusion_texture);
		layman_texture_switch(new->emissive_texture);
	} else {
		layman_texture_switch(NULL);
	}
}

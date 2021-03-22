#include "cglm/cglm.h"
#include "material.h"
#include "utils.h"
#include <stdlib.h>

struct material *material_create(void) {
	struct material *material = malloc(sizeof *material);
	if (!material) {
		return NULL;
	}

	glm_vec4_one(material->base_color_factor);
	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->metallic_factor = 1;
	material->roughness_factor = 1;
	material->normal_texture = NULL;
	material->normal_scale = 1;
	material->occlusion_texture = NULL;
	material->occlusion_strength = 1;
	material->emissive_texture = NULL;
	glm_vec3_one(material->emissive_factor);

	return material;
}

void material_destroy(struct material *material) {
	texture_destroy(material->base_color_texture);
	texture_destroy(material->metallic_roughness_texture);
	texture_destroy(material->normal_texture);
	texture_destroy(material->occlusion_texture);
	texture_destroy(material->emissive_texture);

	free(material);
}

void material_switch(const struct material *new) {
	thread_local static const struct material *current;

	if (new == current) {
		return;
	}

	current = new;

	if (new) {
		texture_switch(new->base_color_texture);
		texture_switch(new->normal_texture);
		texture_switch(new->metallic_roughness_texture);
		texture_switch(new->occlusion_texture);
		texture_switch(new->emissive_texture);
	} else {
		texture_switch(NULL);
	}
}

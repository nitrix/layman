#include "layman2.h"
#include <stdlib.h>

struct layman_material *layman_material_create(void) {
	struct layman_material *material = malloc(sizeof *material);
	if (!material) {
		return NULL;
	}

	material->base_color_factor = LAYMAN_VECTOR_4F(1, 1, 1, 1);
	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->metallic_factor = 0; // FIXME?
	material->roughness_factor = 0; // FIXME?
	material->normal_texture = NULL;
	material->occlusion_texture = NULL;
	material->occlusion_strength = 1;
	material->emissive_texture = NULL;
	material->emissive_factor = LAYMAN_VECTOR_3F(1, 1, 1);

	return material;
}

void layman_material_destroy(struct layman_material *material) {
	if (material->base_color_texture) {
		layman_texture_destroy(material->base_color_texture);
	}

	if (material->metallic_roughness_texture) {
		layman_texture_destroy(material->metallic_roughness_texture);
	}

	if (material->normal_texture) {
		layman_texture_destroy(material->normal_texture);
	}

	if (material->occlusion_texture) {
		layman_texture_destroy(material->occlusion_texture);
	}

	if (material->emissive_texture) {
		layman_texture_destroy(material->emissive_texture);
	}

	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->normal_texture = NULL;
	material->occlusion_texture = NULL;
	material->emissive_texture = NULL;

	free(material);
}

void layman_material_use(const struct layman_material *material) {
	// TODO: More of them.
	layman_texture_switch(material->base_color_texture, NULL);
	layman_texture_switch(material->normal_texture, NULL);
	layman_texture_switch(material->metallic_roughness_texture, NULL);
	layman_texture_switch(material->occlusion_texture, NULL);
	layman_texture_switch(material->emissive_texture, NULL);
}

void layman_material_unuse(const struct layman_material *material) {
	(void) material; // Unused.
}

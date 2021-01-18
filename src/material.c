#include "layman.h"
#include <stdlib.h>

struct layman_material *layman_material_create(void) {
	struct layman_material *material = malloc(sizeof *material);
	if (!material) {
		return NULL;
	}

	material->base_color_factor = LAYMAN_VECTOR_3F(0, 0, 0);
	material->base_color_texture = NULL;
	material->metallic_roughness_texture = NULL;
	material->metallic_factor = 0.0f;
	material->roughness_factor = 0.0f;
	material->normal_texture = NULL;
	material->occlusion_texture = NULL;
	material->emissive_texture = NULL;
	material->emissive_factor = LAYMAN_VECTOR_3F(0, 0, 0);

	return material;
}

void layman_material_destroy(struct layman_material *material) {
	free(material);
}
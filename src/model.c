#include "gltf.h"
#include "layman.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

struct layman_model {
	struct layman_mesh **meshes;
	size_t meshes_count;
};

bool load_meshes(struct layman_model *model, const cgltf_data *gltf) {
	size_t mesh_count = 0;

	// Find out how many meshes there are.
	for (size_t i = 0; i < gltf->meshes_count; i++) {
		for (size_t j = 0; j < gltf->meshes[i].primitives_count; j++) {
			// Only consider meshes with triangle primitives.
			if (gltf->meshes[i].primitives[j].type == cgltf_primitive_type_triangles) {
				mesh_count++;
			}
		}
	}

	model->meshes = malloc(mesh_count * sizeof *model->meshes);
	if (!model->meshes) {
		return false;
	}

	model->meshes_count = mesh_count;
	for (size_t i = 0; i < mesh_count; i++) {
		model->meshes[i] = NULL;
	}

	// Loading the glTF meshes as layman meshes.
	size_t final_mesh_i = 0;
	for (size_t mesh_i = 0; mesh_i < gltf->meshes_count; mesh_i++) {
		for (size_t primitive_i = 0; primitive_i < gltf->meshes[mesh_i].primitives_count; primitive_i++) {
			const cgltf_primitive *primitive = gltf->meshes[mesh_i].primitives + primitive_i;

			// Only consider triangle primitives.
			if (primitive->type != cgltf_primitive_type_triangles) {
				continue;
			}

			// Attributes.
			const float *vertices = NULL;
			size_t vertices_count = 0;
			size_t vertices_stride = 0;
			const float *normals = NULL;
			size_t normals_count = 0;
			size_t normals_stride = 0;
			const unsigned short *indices = NULL;
			size_t indices_count = 0;
			float *uvs = NULL;
			size_t uvs_count = 0;
			size_t uvs_stride = 0;

			for (size_t attribute_i = 0; attribute_i < primitive->attributes_count; attribute_i++) {
				const cgltf_attribute *attribute = primitive->attributes + attribute_i;

				switch (attribute->type) {
					case cgltf_attribute_type_position:
						if (attribute->data->type != cgltf_type_vec3) {
							break;
						}
						vertices = gltf->bin + attribute->data->buffer_view->offset;
						vertices_count = attribute->data->count;
						vertices_stride = attribute->data->stride;
						break;
					case cgltf_attribute_type_normal:
						normals = gltf->bin + attribute->data->buffer_view->offset;
						normals_count = attribute->data->count;
						normals_stride = attribute->data->stride;
						break;
					case cgltf_attribute_type_texcoord:
						if (attribute->data->type != cgltf_type_vec2) {
							break;
						}
						uvs = gltf->bin + attribute->data->buffer_view->offset;
						uvs_count = attribute->data->count;
						uvs_stride = attribute->data->stride;
						break;
					default:
						break;
				}
			}

			if (primitive->indices->component_type != cgltf_component_type_r_16u) {
				fprintf(stderr, "Unsupported mesh indices type\n");
				return false;
			}

			indices = gltf->bin + primitive->indices->buffer_view->offset;
			indices_count = primitive->indices->count;

			// Flip uvs on the Y axis.
			for (size_t i = 0; i < uvs_count; i += 2) {
				uvs[i + 1] = 1 - uvs[i + 1];
			}

			// Create the mesh from raw data.
			model->meshes[final_mesh_i] = layman_mesh_create_from_raw(vertices, vertices_count, vertices_stride, normals, normals_count, normals_stride, uvs, uvs_count, uvs_stride, indices, indices_count);
			if (!model->meshes[final_mesh_i]) {
				return false;
			}

			// Load material.
			struct layman_material *material = layman_material_create();

			// Base color factor.
			cgltf_float *base_color_factor = primitive->material->pbr_metallic_roughness.base_color_factor;
			material->base_color_factor = LAYMAN_VECTOR_3F(base_color_factor[0], base_color_factor[1], base_color_factor[2]);
			// Base color texture
			cgltf_texture *base_color_texture = primitive->material->pbr_metallic_roughness.base_color_texture.texture;
			const void *base_color_texture_data = gltf->bin + base_color_texture->image->buffer_view->offset;
			size_t base_color_texture_size = primitive->material->pbr_metallic_roughness.base_color_texture.texture->image->buffer_view->size;
			material->base_color_texture = layman_texture_create_from_memory(LAYMAN_TEXTURE_ALBEDO, base_color_texture_data, base_color_texture_size);

			final_mesh_i++;
		}
	}

	return true;
}

void unload_meshes(struct layman_model *model) {
	for (size_t i = 0; i < model->meshes_count; i++) {
		if (model->meshes[i]) {
			layman_mesh_destroy(model->meshes[i]);
		}
	}

	free(model->meshes);

	// Reset the relevant fields.
	// This is because unloading can occur even though nothing is already loaded.
	// Also, there can be multipel loading/unloading cycles per model lifespan.
	model->meshes = NULL;
	model->meshes_count = 0;
}

struct layman_model *layman_model_load(const char *filepath) {
	struct layman_model *model = malloc(sizeof *model);
	if (!model) {
		return NULL;
	}

	cgltf_data *gltf = NULL;
	cgltf_options options = {0};
	cgltf_result result = cgltf_parse_file(&options, filepath, &gltf);

	if (result != cgltf_result_success) {
		free(model);
		return NULL;
	}

	model->meshes = NULL;
	model->meshes_count = 0;

	bool loaded = load_meshes(model, gltf);

	cgltf_free(gltf);

	if (!loaded) {
		layman_model_destroy(model);
		return NULL;
	}

	return model;
}

void layman_model_destroy(struct layman_model *model) {
	unload_meshes(model);
	free(model);
}

void layman_model_render(const struct layman_model *model) {
	for (size_t i = 0; i < model->meshes_count; i++) {
		struct layman_mesh *mesh = model->meshes[i];
		layman_mesh_render(mesh);
	}
}
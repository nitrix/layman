#include "client.h"

bool load_meshes(struct model *model, const cgltf_data *gltf) {
	size_t mesh_count = 0;

	// Find out how many meshes there are.
	for (size_t i = 0; i < gltf->meshes_count; i++) {
		for (size_t j = 0; j < gltf->meshes[i].primitives_count; j++) {
			// But only consider meshes with triangle primitives.
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
		mesh_init(&model->meshes[i]);
	}

	// Loading the glTF meshes into our meshes.
	size_t final_mesh_i = 0;
	for (size_t mesh_i = 0; mesh_i < gltf->meshes_count; mesh_i++) {
		for (size_t primitive_i = 0; primitive_i < gltf->meshes[mesh_i].primitives_count; primitive_i++) {
			const cgltf_primitive *primitive = gltf->meshes[mesh_i].primitives + primitive_i;

			// Only support triangle primitives.
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
			const float *uvs = NULL;
			size_t uvs_count = 0;
			size_t uvs_stride = 0;
			const float *tangents = NULL;
			size_t tangents_count = 0;
			size_t tangents_stride = 0;

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
					    if (attribute->data->type != cgltf_type_vec3) {
						    break;
					    }

					    normals = gltf->bin + attribute->data->buffer_view->offset;
					    normals_count = attribute->data->count;
					    normals_stride = attribute->data->stride;
					    break;

				    case cgltf_attribute_type_tangent:
					    if (attribute->data->type != cgltf_type_vec4) {
						    break;
					    }

					    tangents = gltf->bin + attribute->data->buffer_view->offset;
					    tangents_count = attribute->data->count;
					    tangents_stride = attribute->data->stride;
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

			struct mesh *mesh = &model->meshes[final_mesh_i++];

			mesh_provide_vertices(mesh, vertices, vertices_count, vertices_stride);
			mesh_provide_normals(mesh, normals, normals_count, normals_stride);
			mesh_provide_uvs(mesh, uvs, uvs_count, uvs_stride);
			mesh_provide_indices(mesh, indices, indices_count);
			mesh_provide_tangents(mesh, tangents, tangents_count, tangents_stride);

			// FIXME: Handle allocation failures of the textures below.

			// Metallic/roughness workflow (optional).
			if (primitive->material->has_pbr_metallic_roughness) {
				cgltf_pbr_metallic_roughness *mr = &primitive->material->pbr_metallic_roughness;

				// Base color factor.
				glm_vec3_copy(mr->base_color_factor, mesh->material.base_color_factor);

				// Base color texture (optional).
				if (mr->base_color_texture.texture) {
					struct texture *texture = malloc(sizeof *texture);
					texture_init_from_memory(texture, TEXTURE_KIND_ALBEDO,
						gltf->bin + mr->base_color_texture.texture->image->buffer_view->offset,
						mr->base_color_texture.texture->image->buffer_view->size
					);
					mesh->material.base_color_texture = texture;
				}

				// Metallic/roughness texture (optional).
				if (mr->metallic_roughness_texture.texture) {
					struct texture *texture = malloc(sizeof *texture);
					texture_init_from_memory(texture, TEXTURE_KIND_METALLIC_ROUGHNESS,
						gltf->bin + mr->metallic_roughness_texture.texture->image->buffer_view->offset,
						mr->metallic_roughness_texture.texture->image->buffer_view->size
					);
					mesh->material.metallic_roughness_texture = texture;
				}

				// Metallic factor.
				mesh->material.metallic_factor = mr->metallic_factor;

				// Roughness factor.
				mesh->material.roughness_factor = mr->roughness_factor;
			}

			// Normal texture (optional).
			if (primitive->material->normal_texture.texture) {
				struct texture *texture = malloc(sizeof *texture);
				texture_init_from_memory(texture, TEXTURE_KIND_NORMAL,
					gltf->bin + primitive->material->normal_texture.texture->image->buffer_view->offset,
					primitive->material->normal_texture.texture->image->buffer_view->size
				);
				mesh->material.normal_texture = texture;
			}

			// Occlusion texture (optional).
			if (primitive->material->occlusion_texture.texture) {
				struct texture *texture = malloc(sizeof *texture);
				texture_init_from_memory(texture, TEXTURE_KIND_OCCLUSION,
					gltf->bin + primitive->material->occlusion_texture.texture->image->buffer_view->offset,
					primitive->material->occlusion_texture.texture->image->buffer_view->size
				);
				mesh->material.occlusion_texture = texture;
			}

			// Emissive factor.
			glm_vec3_copy(primitive->material->emissive_factor, mesh->material.emissive_factor);

			// Emissive texture (optional).
			if (primitive->material->emissive_texture.texture) {
				struct texture *texture = malloc(sizeof *texture);
				texture_init_from_memory(texture, TEXTURE_KIND_EMISSION,
					gltf->bin + primitive->material->emissive_texture.texture->image->buffer_view->offset,
					primitive->material->emissive_texture.texture->image->buffer_view->size
				);
				mesh->material.emissive_texture = texture;
			}
		}
	}

	return true;
}

struct model *model_load(const char *filepath) {
	struct model *model = malloc(sizeof *model);
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

	// Model name is the filepath for now.
	model->name = strdup(filepath);

	bool loaded = load_meshes(model, gltf);

	cgltf_free(gltf);

	if (!loaded) {
		model_destroy(model);
		return NULL;
	}

	return model;
}

void model_destroy(struct model *model) {
	if (model->meshes) {
		for (size_t i = 0; i < model->meshes_count; i++) {
			mesh_fini(&model->meshes[i]);
		}

		free(model->meshes);
		model->meshes = NULL;
		model->meshes_count = 0;
	}

	free(model->name);
	free(model);
}

#include "client.h"

INCBIN(shaders_pbr_main_vert, "../shaders/pbr/main.vert");
INCBIN(shaders_pbr_main_frag, "../shaders/pbr/main.frag");

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
				fprintf(stderr, "Unsupported primitives\n");
				return false;
			}

			struct shader_options options = {
				.tonemap_uncharted = true,
				.use_hdr = true,
				.use_ibl = true,
				// .use_punctual = true,
				// .light_count = MAX_LIGHTS,
			};

			// Attributes.
			const float *vertices = NULL;
			size_t vertices_count = 0;
			size_t vertices_stride = 0;
			const float *normals = NULL;
			size_t normals_count = 0;
			size_t normals_stride = 0;
			const void *indices = NULL;
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

					    options.has_normals = true;

					    normals = gltf->bin + attribute->data->buffer_view->offset;
					    normals_count = attribute->data->count;
					    normals_stride = attribute->data->stride;
					    break;

				    case cgltf_attribute_type_tangent:
					    if (attribute->data->type != cgltf_type_vec4) {
						    break;
					    }

					    options.has_tangents = true;

					    tangents = gltf->bin + attribute->data->buffer_view->offset;
					    tangents_count = attribute->data->count;
					    tangents_stride = attribute->data->stride;
					    break;

				    case cgltf_attribute_type_texcoord:
					    if (attribute->data->type != cgltf_type_vec2) {
						    break;
					    }

					    options.has_uv_set1 = true;

					    uvs = gltf->bin + attribute->data->buffer_view->offset;
					    uvs_count = attribute->data->count;
					    uvs_stride = attribute->data->stride;
					    break;

				    default:
					    break;
				}
			}

			indices = gltf->bin + primitive->indices->buffer_view->offset;
			indices_count = primitive->indices->count;

			struct mesh *mesh = &model->meshes[final_mesh_i++];

			switch (primitive->indices->component_type) {
			    case cgltf_component_type_r_8: mesh->indices_type = GL_BYTE; break;
			    case cgltf_component_type_r_8u: mesh->indices_type = GL_UNSIGNED_BYTE; break;
			    case cgltf_component_type_r_16: mesh->indices_type = GL_SHORT; break;
			    case cgltf_component_type_r_16u: mesh->indices_type = GL_UNSIGNED_SHORT; break;
			    case cgltf_component_type_r_32u: mesh->indices_type = GL_UNSIGNED_INT; break;
			    default:
				    fprintf(stderr, "Unsupported mesh indices type\n");
				    return false;
			}

			mesh_provide_vertices(mesh, vertices, vertices_count, vertices_stride);
			mesh_provide_normals(mesh, normals, normals_count, normals_stride);
			mesh_provide_uvs(mesh, uvs, uvs_count, uvs_stride);
			mesh_provide_indices(mesh, indices, indices_count, mesh->indices_type);
			mesh_provide_tangents(mesh, tangents, tangents_count, tangents_stride);

			// FIXME: Handle allocation failures of the textures below.

			mesh->material.name = strdup(primitive->material->name);

			// Metallic/roughness workflow (optional).
			if (primitive->material->has_pbr_metallic_roughness) {
				cgltf_pbr_metallic_roughness *mr = &primitive->material->pbr_metallic_roughness;

				options.material_metallicroughness = true;

				// Base color factor.
				glm_vec3_copy(mr->base_color_factor, mesh->material.base_color_factor);

				// Base color texture (optional).
				if (mr->base_color_texture.texture) {
					options.has_base_color_map = true;

					struct texture *texture = malloc(sizeof *texture);
					texture_init_from_memory(texture, TEXTURE_KIND_ALBEDO,
						gltf->bin + mr->base_color_texture.texture->image->buffer_view->offset,
						mr->base_color_texture.texture->image->buffer_view->size
					);
					mesh->material.base_color_texture = texture;
				}

				// Metallic/roughness texture (optional).
				if (mr->metallic_roughness_texture.texture) {
					options.has_metallic_roughness_map = true;

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
				options.has_normal_map = true;

				struct texture *texture = malloc(sizeof *texture);
				texture_init_from_memory(texture, TEXTURE_KIND_NORMAL,
					gltf->bin + primitive->material->normal_texture.texture->image->buffer_view->offset,
					primitive->material->normal_texture.texture->image->buffer_view->size
				);
				mesh->material.normal_texture = texture;
			}

			// Occlusion texture (optional).
			if (primitive->material->occlusion_texture.texture) {
				options.has_occlusion_map = true;

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
				options.has_emissive_map = true;

				struct texture *texture = malloc(sizeof *texture);
				texture_init_from_memory(texture, TEXTURE_KIND_EMISSION,
					gltf->bin + primitive->material->emissive_texture.texture->image->buffer_view->offset,
					primitive->material->emissive_texture.texture->image->buffer_view->size
				);
				mesh->material.emissive_texture = texture;
			}

			// Initial transform.
			for (size_t i = 0; i < gltf->nodes_count; i++) {
				cgltf_node *node = &gltf->nodes[i];

				// Find a node corresponding to our mesh.
				if (node->mesh == gltf->meshes + mesh_i) {
					if (node->has_matrix) {
						glm_mat4_ucopy((vec4 *) node->matrix, mesh->initial_transform);
					} else {
						if (node->has_translation) {
							glm_translate(mesh->initial_transform, node->translation);
						}

						if (node->has_rotation) {
							glm_quat_rotate(mesh->initial_transform, (versor) { node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]}, mesh->initial_transform);
						}

						if (node->has_scale) {
							glm_scale(mesh->initial_transform, node->scale);
						}
					}
				}
			}

			// Shader.
			// TODO: Shader caching based on the #defines that it needs. Each mesh having their own shader is a bit wasteful otherwise.
			mesh->shader = shader_load_from_memory(&options, shaders_pbr_main_vert_data, shaders_pbr_main_vert_size, shaders_pbr_main_frag_data, shaders_pbr_main_frag_size, NULL, 0);
			if (!mesh->shader) {
				return false;
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
	model->filepath = strdup(filepath);

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

	free(model->filepath);
	free(model);
}

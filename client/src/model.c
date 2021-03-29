#include "cglm/cglm.h"
#include "gltf.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include "utils.h"
#include <stdlib.h>

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
		model->meshes[i] = NULL;
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

			// Create the mesh from raw data.
			struct mesh *mesh = mesh_create_from_raw(
					// Vertices.
					vertices, vertices_count, vertices_stride,
					// Normals.
					normals, normals_count, normals_stride,
					// UVs.
					uvs, uvs_count, uvs_stride,
					// Indices.
					indices, indices_count,
					// Tangents.
					tangents, tangents_count, tangents_stride);

			if (!mesh) {
				return false;
			}

			// Load material.
			struct material *material = material_create();

			// Base color factor.
			cgltf_float *base_color_factor = primitive->material->pbr_metallic_roughness.base_color_factor;
			VEC4_ASSIGN(material->base_color_factor, base_color_factor[0], base_color_factor[1], base_color_factor[2], base_color_factor[3]);
			// Base color texture
			cgltf_texture *base_color_texture = primitive->material->pbr_metallic_roughness.base_color_texture.texture;
			const void *base_color_texture_data = gltf->bin + base_color_texture->image->buffer_view->offset;
			size_t base_color_texture_size = primitive->material->pbr_metallic_roughness.base_color_texture.texture->image->buffer_view->size;
			material->base_color_texture = texture_create_from_memory(TEXTURE_KIND_ALBEDO, base_color_texture_data, base_color_texture_size);
			// Normal texture
			cgltf_texture *normal_texture = primitive->material->normal_texture.texture;
			const void *normal_texture_data = gltf->bin + normal_texture->image->buffer_view->offset;
			size_t normal_texture_size = primitive->material->normal_texture.texture->image->buffer_view->size;
			material->normal_texture = texture_create_from_memory(TEXTURE_KIND_NORMAL, normal_texture_data, normal_texture_size);
			// Metallic/roughness texture
			cgltf_texture *metallic_roughness_texture = primitive->material->pbr_metallic_roughness.metallic_roughness_texture.texture;
			const void *metallic_roughness_texture_data = gltf->bin + metallic_roughness_texture->image->buffer_view->offset;
			size_t metallic_roughness_texture_size = primitive->material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->buffer_view->size;
			material->metallic_roughness_texture = texture_create_from_memory(TEXTURE_KIND_METALLIC_ROUGHNESS, metallic_roughness_texture_data, metallic_roughness_texture_size);
			// Occlusion texture
			cgltf_texture *occlusion_texture = primitive->material->occlusion_texture.texture;
			const void *occlusion_texture_data = gltf->bin + occlusion_texture->image->buffer_view->offset;
			size_t occlusion_texture_size = primitive->material->occlusion_texture.texture->image->buffer_view->size;
			material->occlusion_texture = texture_create_from_memory(TEXTURE_KIND_OCCLUSION, occlusion_texture_data, occlusion_texture_size);
			// Emission texture
			cgltf_texture *emission_texture = primitive->material->emissive_texture.texture;
			const void *emission_texture_data = gltf->bin + emission_texture->image->buffer_view->offset;
			size_t emission_texture_size = primitive->material->emissive_texture.texture->image->buffer_view->size;
			material->emissive_texture = texture_create_from_memory(TEXTURE_KIND_EMISSION, emission_texture_data, emission_texture_size);

			mesh->material = material; // FIXME: Leaking the material?!

			model->meshes[final_mesh_i++] = mesh;
		}
	}

	return true;
}

void unload_meshes(struct model *model) {
	for (size_t i = 0; i < model->meshes_count; i++) {
		if (model->meshes[i]) {
			mesh_destroy(model->meshes[i]);
		}
	}

	free(model->meshes);

	// Reset the relevant fields.
	// This is because unloading can occur even though nothing is already loaded.
	// Also, there can be multipel loading/unloading cycles per model lifespan.
	model->meshes = NULL;
	model->meshes_count = 0;
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

	bool loaded = load_meshes(model, gltf);

	cgltf_free(gltf);

	if (!loaded) {
		model_destroy(model);
		return NULL;
	}

	return model;
}

void model_destroy(struct model *model) {
	if (model) {
		unload_meshes(model);
	}

	free(model);
}

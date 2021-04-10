#ifndef SHADER_H
#define SHADER_H

#include "camera.h"
#include "environment.h"
#include "light.h"
#include "material.h"

struct shader {
	GLuint program_id;

	// Material uniforms.
	GLint uniform_base_color_factor;
	GLint uniform_base_color_sampler;
	GLint uniform_normal_sampler;
	GLint uniform_normal_scale;
	GLint uniform_metallic_roughness_sampler;
	GLint uniform_metallic_factor;
	GLint uniform_roughness_factor;
	GLint uniform_occlusion_sampler;
	GLint uniform_occlusion_strength;
	GLint uniform_emissive_sampler;
	GLint uniform_emissive_factor;

	// Environment IBL.
	GLint uniform_environment_mip_count;
	GLint uniform_environment_lambertian;
	GLint uniform_environment_ggx;
	GLint uniform_environment_ggx_lut;
	GLint uniform_environment_charlie;
	GLint uniform_environment_charlie_lut;

	// Camera uniforms.
	GLint uniform_camera;

	// Light uniforms.
	GLint uniform_lights_type[MAX_LIGHTS];
	GLint uniform_lights_position[MAX_LIGHTS];
	GLint uniform_lights_direction[MAX_LIGHTS];
	GLint uniform_lights_color[MAX_LIGHTS];
	GLint uniform_lights_intensity[MAX_LIGHTS];

	// Model/View/Projection (MVP matrices).
	GLint uniform_view_projection_matrix;
	GLint uniform_model_matrix;
	GLint uniform_normal_matrix;
	GLint uniform_exposure;
};

struct shader_options {
	// Attributes.
	bool has_normals;
	bool has_uv_set1;
	bool has_tangents;
	bool has_weight_set1;
	bool has_joint_set1;
	bool has_color_vec3;
	bool has_color_vec4;

	// Textures.
	bool has_base_color_map;
	bool has_normal_map;
	bool has_occlusion_map;
	bool has_emissive_map;
	bool has_metallic_roughness_map;

	// FIXME: Enum for the workflow?
	bool material_metallicroughness;
	bool material_specularglossiness;

	// Lighting.
	bool material_unlit;
	bool use_hdr;
	bool use_ibl;
	bool use_punctual;
	int light_count;

	// Skinning.
	bool use_skinning;
	int joint_count;

	// Tone-mapping.
	// FIXME: Enum for this?
	bool tonemap_uncharted;
	bool tonemap_hejlrichard;
	bool tonemap_aces;

	// Debugging.
	// FIXME: Enum for this?
	bool debug_output;
	bool debug_basecolor;
	bool debug_normal;
	bool debug_tangent;
	bool debug_metallic;
	bool debug_roughness;
	bool debug_occlusion;
	bool debug_fdiffuse;
	bool debug_fspecular;
	bool debug_femissive;
};

struct shader *shader_load_from_files(const struct shader_options *options, const char *vertex_filepath, const char *fragment_filepath, const char *compute_filepath);
struct shader *shader_load_from_memory(const struct shader_options *options, const unsigned char *vertex_content, size_t vertex_length, const unsigned char *fragment_content, size_t fragment_length, const unsigned char *compute_content, size_t compute_length);
void shader_destroy(struct shader *shader);

void shader_bind_uniform_material(const struct shader *shader, const struct material *material);
void shader_bind_uniform_camera(const struct shader *shader, const struct camera *camera);
void shader_bind_uniform_lights(const struct shader *shader, const struct light **lights, size_t count);
void shader_bind_uniform_environment(const struct shader *shader, const struct environment *environment);
void shader_bind_uniform_mvp(const struct shader *shader, mat4 view_projection_matrix, mat4 model_matrix, float exposure);

#endif

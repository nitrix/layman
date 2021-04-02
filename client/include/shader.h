#ifndef SHADER_H
#define SHADER_H

#include "camera.h"
#include "environment.h"
#include "light.h"
#include <glad/glad.h>

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

void shader_switch(const struct shader *shader);

// TODO: Documentation.
struct shader *shader_load_from_files(const char *vertex_filepath, const char *fragment_filepath, const char *compute_filepath);

// TODO: Documentation.
struct shader *shader_load_from_memory(const unsigned char *vertex_content, size_t vertex_length, const unsigned char *fragment_content, size_t fragment_length, const unsigned char *compute_content, size_t compute_length);

// TODO: Documentation.
void shader_destroy(struct shader *shader);

void shader_bind_uniform_material(const struct shader *shader, const struct material *material);
void shader_bind_uniform_camera(const struct shader *shader, const struct camera *camera);
void shader_bind_uniform_lights(const struct shader *shader, const struct light **lights, size_t count);
void shader_bind_uniform_environment(const struct shader *shader, const struct environment *environment);
void shader_bind_uniform_mvp(const struct shader *shader, mat4 view_projection_matrix, mat4 model_matrix, float exposure);

#endif

#include "layman.h"

// FIXME: This function is a disaster.
static char *read_shader_file(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		return NULL;
	}

	char *content = NULL;
	size_t content_size = 0;
	char line[1024];

	while (!feof(file)) {
		char *ok = fgets(line, sizeof line, file);
		if (!ok) {
			break;
		}

		size_t read = strlen(line);

		if (strncmp(line, "#include", 8) == 0) {
			const char *bracket_start = strchr(line, '<');
			const char *bracket_end = strchr(line, '>');

			const char *dir_end = strrchr(filepath, '/') + 1;
			size_t length = snprintf(NULL, 0, "%.*s/%.*s", (int) (dir_end - filepath - 1), filepath, (int) (bracket_end - bracket_start - 1), bracket_start + 1);
			char *new_filepath = malloc(length + 1);
			if (!new_filepath) {
				free(content);
				fclose(file);
				return NULL;
			}

			sprintf(new_filepath, "%.*s/%.*s", (int) (dir_end - filepath - 1), filepath, (int) (bracket_end - bracket_start - 1), bracket_start + 1);

			char *other_content = read_shader_file(new_filepath);
			if (!other_content) {
				free(content);
				fclose(file);
				return NULL;
			}

			size_t other_content_size = strlen(other_content);

			char *new_content = realloc(content, content_size + other_content_size);
			if (!new_content) {
				free(content);
				fclose(file);
				return NULL;
			}

			memcpy(new_content + content_size, other_content, other_content_size);

			content = new_content;
			content_size += other_content_size;

			free(new_filepath);
		} else {
			char *new_content = realloc(content, content_size + read);
			if (!new_content) {
				free(content);
				fclose(file);
				return NULL;
			}

			// Append to the new content.
			memcpy(new_content + content_size, line, read);

			content = new_content;
			content_size += read;
		}
	}

	// Resize to final size and null terminate.

	char *new_content = realloc(content, content_size + 1);
	if (!new_content) {
		fclose(file);
		free(content);
		return NULL;
	}

	content = new_content;
	content[content_size] = '\0';

	fclose(file);

	return content;
}

/**
 * Compiles a shader.
 *
 * @param[in] type The type of shader.
 *      - GL_COMPUTE_SHADER
 *      - GL_VERTEX_SHADER
 *      - GL_TESS_CONTROL_SHADER
 *      - GL_TESS_EVALUATION_SHADER
 *      - GL_GEOMETRY_SHADER
 *      - GL_FRAGMENT_SHADER
 * @param[in] source The source of the shader.
 *
 * @return The compiled shader id or `0` on error.
 */
static GLuint compile_shader(GLenum type, const char *filepath) {
	GLuint shader_id = glCreateShader(type);
	if (shader_id == 0) {
		return 0;
	}

	char *content = read_shader_file(filepath);
	if (!content) {
		glDeleteShader(shader_id);
		return 0;
	}

	const char *prefix =
	        "#version 410 core\n\n"

	        // TODO: All of the has should be set accordinly to what the mesh actually has, not hardcoded.

	        // Attributes.
	        "#define HAS_NORMALS\n"
	        "#define HAS_UV_SET1\n"
	        // "#define HAS_TANGENTS\n"

	        // Textures.
	        "#define HAS_BASE_COLOR_MAP\n"
	        "#define HAS_NORMAL_MAP\n"
	        "#define HAS_OCCLUSION_MAP\n"
	        "#define HAS_EMISSIVE_MAP\n"
	        "#define HAS_METALLIC_ROUGHNESS_MAP\n"

	        // Material workflow.
	        "#define MATERIAL_METALLICROUGHNESS\n"
	        // "#define MATERIAL_SPECULARGLOSSINESS\n"

	        // Lighting.
	        // "#define MATERIAL_UNLIT\n"
	        "#define USE_HDR\n"
	        "#define USE_IBL\n"
	        // "#define USE_PUNCTUAL\n"
	        "#define LIGHT_COUNT " EVAL_TO_STR(MAX_LIGHTS) "\n"

	        // Debugging.
	        // "#define DEBUG_OUTPUT\n"
	        // "#define DEBUG_BASECOLOR\n"
	        // "#define DEBUG_NORMAL\n"
	        // "#define DEBUG_METALLIC\n"
	        // "#define DEBUG_ROUGHNESS\n"
	        // "#define DEBUG_OCCLUSION\n"
	        // "#define DEBUG_FDIFFUSE\n"
	        // "#define DEBUG_FSPECULAR\n"
	        // "#define DEBUG_FEMISSIVE\n"

	        "#define DUMMY 1\n\n";

	size_t new_length = snprintf(NULL, 0, "%s\n%s", prefix, content);
	char *new_content = malloc(new_length + 1);
	if (!new_content) {
		free(content);
		glDeleteShader(shader_id);
		return 0;
	}

	sprintf(new_content, "%s\n%s", prefix, content);
	free(content);

	const char *const source = new_content;
	glShaderSource(shader_id, 1, &source, NULL);

	glCompileShader(shader_id);

	GLint success;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (success != GL_TRUE) {
		GLint info_log_size;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
		char info_log[info_log_size];
		glGetShaderInfoLog(shader_id, info_log_size, NULL, info_log);

		fprintf(stderr, "Shader compilation error!\n%s: %s\n", filepath, info_log);

		return 0;
	}

	return shader_id;
}

static void find_uniforms(struct layman_shader *shader) {
	shader->uniform_base_color_factor = glGetUniformLocation(shader->program_id, "u_BaseColorFactor");
	shader->uniform_base_color_sampler = glGetUniformLocation(shader->program_id, "u_BaseColorSampler");
	shader->uniform_normal_sampler = glGetUniformLocation(shader->program_id, "u_NormalSampler");
	shader->uniform_normal_scale = glGetUniformLocation(shader->program_id, "u_NormalScale");
	shader->uniform_metallic_roughness_sampler = glGetUniformLocation(shader->program_id, "u_MetallicRoughnessSampler");
	shader->uniform_metallic_factor = glGetUniformLocation(shader->program_id, "u_MetallicFactor");
	shader->uniform_roughness_factor = glGetUniformLocation(shader->program_id, "u_RoughnessFactor");
	shader->uniform_occlusion_sampler = glGetUniformLocation(shader->program_id, "u_OcclusionSampler");
	shader->uniform_occlusion_strength = glGetUniformLocation(shader->program_id, "u_OcclusionStrength");
	shader->uniform_emissive_sampler = glGetUniformLocation(shader->program_id, "u_EmissiveSampler");
	shader->uniform_emissive_factor = glGetUniformLocation(shader->program_id, "u_EmissiveFactor");
	shader->uniform_camera = glGetUniformLocation(shader->program_id, "u_Camera");

	shader->uniform_environment_mip_count = glGetUniformLocation(shader->program_id, "u_MipCount");
	shader->uniform_environment_lambertian = glGetUniformLocation(shader->program_id, "u_LambertianEnvSampler");
	shader->uniform_environment_ggx = glGetUniformLocation(shader->program_id, "u_GGXEnvSampler");
	shader->uniform_environment_ggx_lut = glGetUniformLocation(shader->program_id, "u_GGXLUT");
	shader->uniform_environment_charlie = glGetUniformLocation(shader->program_id, "u_CharlieEnvSampler");
	shader->uniform_environment_charlie_lut = glGetUniformLocation(shader->program_id, "u_CharlieLUT");

	char name[64];
	for (size_t i = 0; i < MAX_LIGHTS; i++) {
		sprintf(name, "u_Lights[%zu].type", i);
		shader->uniform_lights_type[i] = glGetUniformLocation(shader->program_id, name);

		sprintf(name, "u_Lights[%zu].position", i);
		shader->uniform_lights_position[i] = glGetUniformLocation(shader->program_id, name);

		sprintf(name, "u_Lights[%zu].direction", i);
		shader->uniform_lights_direction[i] = glGetUniformLocation(shader->program_id, name);

		sprintf(name, "u_Lights[%zu].color", i);
		shader->uniform_lights_color[i] = glGetUniformLocation(shader->program_id, name);

		sprintf(name, "u_Lights[%zu].intensity", i);
		shader->uniform_lights_intensity[i] = glGetUniformLocation(shader->program_id, name);
	}
}

struct layman_shader *layman_shader_load_from_files(const char *vertex_filepath, const char *fragment_filepath, const char *compute_filepath) {
	GLuint vertex_shader_id = 0;
	GLuint fragment_shader_id = 0;
	GLuint compute_shader_id = 0;
	bool something_went_wrong = false;

	if (vertex_filepath) {
		vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_filepath);
		if (!vertex_shader_id) {
			something_went_wrong = true;
		}
	}

	if (fragment_filepath) {
		fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_filepath);
		if (!fragment_shader_id) {
			something_went_wrong = true;
		}
	}

	if (compute_filepath) {
		compute_shader_id = compile_shader(GL_COMPUTE_SHADER, compute_filepath);
		if (!compute_shader_id) {
			something_went_wrong = true;
		}
	}

	if (something_went_wrong) {
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		glDeleteShader(compute_shader_id);
		return NULL;
	}

	GLuint program_id = glCreateProgram();
	if (!program_id) {
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		glDeleteShader(compute_shader_id);
		return NULL;
	}

	if (vertex_shader_id) {
		glAttachShader(program_id, vertex_shader_id);
	}

	if (fragment_shader_id) {
		glAttachShader(program_id, fragment_shader_id);
	}

	if (compute_shader_id) {
		glAttachShader(program_id, compute_shader_id);
	}

	// Bind attributes. Must be before linkage.
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_POSITION, "a_Position");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_UV, "a_UV1");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_NORMAL, "a_Normal");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_TANGENT, "a_Tangent");

	glLinkProgram(program_id);

	GLint success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (success != GL_TRUE) {
		GLint info_log_size;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_size);
		char info_log[info_log_size];
		glGetProgramInfoLog(program_id, info_log_size, NULL, info_log);

		fprintf(stderr, "Shader linking error: %s\n", info_log);

		return 0;
	}

	// Flag shaders for deletion as soon as they get detached (happens upon program deletion).
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
	glDeleteShader(compute_shader_id);

	struct layman_shader *shader = malloc(sizeof *shader);
	if (!shader) {
		glDeleteProgram(program_id);
		return NULL;
	}

	shader->program_id = program_id;

	layman_shader_switch(shader);
	find_uniforms(shader);

	return shader;
}

void layman_shader_destroy(struct layman_shader *shader) {
	glDeleteProgram(shader->program_id);
	free(shader);
}

void layman_shader_switch(const struct layman_shader *new) {
	thread_local static const struct layman_shader *current;

	if (current == new) {
		return;
	}

	current = new;

	if (new) {
		glUseProgram(new->program_id);
	}
}

void layman_shader_bind_uniform_material(const struct layman_shader *shader, const struct layman_material *material) {
	layman_shader_switch(shader);

	glUniform4fv(shader->uniform_base_color_factor, 1, material->base_color_factor);
	glUniform1i(shader->uniform_base_color_sampler, material->base_color_texture->kind);
	glUniform1i(shader->uniform_metallic_roughness_sampler, material->metallic_roughness_texture->kind);
	glUniform1f(shader->uniform_metallic_factor, material->metallic_factor);
	glUniform1f(shader->uniform_roughness_factor, material->roughness_factor);
	glUniform1i(shader->uniform_normal_sampler, material->normal_texture->kind);
	glUniform1f(shader->uniform_normal_scale, material->normal_scale);
	glUniform1i(shader->uniform_occlusion_sampler, material->occlusion_texture->kind);
	glUniform1f(shader->uniform_occlusion_strength, material->occlusion_strength);
	glUniform1i(shader->uniform_emissive_sampler, material->emissive_texture->kind);
	glUniform3fv(shader->uniform_emissive_factor, 1, material->emissive_factor);
}

void layman_shader_bind_uniform_environment(const struct layman_shader *shader, const struct layman_environment *environment) {
	layman_shader_switch(shader);

	glUniform1i(shader->uniform_environment_mip_count, environment->mip_count);
	glUniform1i(shader->uniform_environment_lambertian, environment->lambertian->kind);
	glUniform1i(shader->uniform_environment_ggx, environment->ggx->kind);
	glUniform1i(shader->uniform_environment_ggx_lut, environment->ggx_lut->kind);
	glUniform1i(shader->uniform_environment_charlie, environment->charlie->kind);
	glUniform1i(shader->uniform_environment_charlie_lut, environment->charlie_lut->kind);
}

void layman_shader_bind_uniform_camera(const struct layman_shader *shader, const struct layman_camera *camera) {
	layman_shader_switch(shader);

	glUniform3fv(shader->uniform_camera, 1, camera->position);
}

void layman_shader_bind_uniform_lights(const struct layman_shader *shader, const struct layman_light **lights, size_t count) {
	layman_shader_switch(shader);

	for (size_t i = 0; i < count; i++) {
		// TODO: This doesn't clean up old light uniforms when they're removed!
		if (i >= MAX_LIGHTS) {
			break;
		}

		const struct layman_light *light = lights[i];

		switch (light->type) {
		    case LAYMAN_LIGHT_TYPE_DIRECTIONAL:
			    glUniform1i(shader->uniform_lights_type[i], light->type);
			    glUniform3fv(shader->uniform_lights_position[i], 1, light->position);
			    glUniform3fv(shader->uniform_lights_direction[i], 1, light->direction);
			    glUniform3fv(shader->uniform_lights_color[i], 1, light->color);
			    glUniform1f(shader->uniform_lights_intensity[i], light->intensity);
			    break;

		    case LAYMAN_LIGHT_TYPE_POINT:
			    glUniform1i(shader->uniform_lights_type[i], light->type);
			    glUniform3fv(shader->uniform_lights_position[i], 1, light->position);
			    glUniform3fv(shader->uniform_lights_color[i], 1, light->color);
			    glUniform1f(shader->uniform_lights_intensity[i], light->intensity);
			    break;

		    // TODO: The other types of lights.
		    default: break;
		}
	}
}

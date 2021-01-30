#include "layman2.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FIXME: This function is a disaster.
char *read_shader_file(const char *filepath) {
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
	        "#version 410\n"

	        "#define HAS_BASE_COLOR_MAP 1\n"
	        "#define HAS_NORMALS 1\n"
	        "#define HAS_TANGENTS 1\n"
	        "#define HAS_NORMAL_MAP 1\n"
	        "#define HAS_OCCLUSION_MAP 1\n"
	        "#define HAS_EMISSIVE_MAP 1\n"
	        "#define HAS_UV_SET1 1\n"
	        "#define MATERIAL_METALLICROUGHNESS 1\n"
	        // "#define MATERIAL_UNLIT 1\n"

	        "#define DEBUG_OUTPUT 1\n"
	        // "#define DEBUG_BASECOLOR 1\n"
	        // "#define DEBUG_NORMAL 1\n"
	        // "#define DEBUG_OCCLUSION 1\n"
	        "#define DEBUG_FDIFFUSE 1\n"
	        // "#define DEBUG_FSPECULAR 1\n"
	        // "#define DEBUG_FEMISSIVE 1\n"

	        "#define DUMMY 1\n";

	size_t new_length = snprintf(NULL, 0, "%s\n%s", prefix, content);
	char *new_content = malloc(new_length + 1);
	if (!new_content) {
		// TODO: Handle error.
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
	shader->uniform_metallic_roughness_sampler = glGetUniformLocation(shader->program_id, "u_MetallicRoughnessSampler");
	shader->uniform_occlusion_sampler = glGetUniformLocation(shader->program_id, "u_OcclusionSampler");
	shader->uniform_emissive_sampler = glGetUniformLocation(shader->program_id, "u_EmissiveSampler");
	shader->uniform_emissive_factor = glGetUniformLocation(shader->program_id, "u_EmissiveFactor");
}

struct layman_shader *layman_shader_load_from_file(const char *vertex_filepath, const char *fragment_filepath) {
	GLint vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_filepath);
	GLint fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_filepath);
	if (!vertex_shader_id || !fragment_shader_id) {
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		return NULL;
	}

	GLuint program_id = glCreateProgram();
	if (!program_id) {
		glDeleteShader(vertex_shader_id);
		glDeleteShader(fragment_shader_id);
		return NULL;
	}

	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);

	// Bind attributes. Must be before linkage.
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_POSITION, "a_Position");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_UV, "a_UV1");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_NORMAL, "a_Normal");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_TANGENT, "a_Tangent");
	// glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_BITANGENT, "bitangent");

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

	struct layman_shader *shader = malloc(sizeof *shader);
	if (!shader) {
		glDeleteProgram(program_id);
		return NULL;
	}

	shader->program_id = program_id;

	find_uniforms(shader);

	return shader;
}

void layman_shader_destroy(struct layman_shader *shader) {
	glDeleteProgram(shader->program_id);
	free(shader);
}

void layman_shader_use(const struct layman_shader *shader) {
	glUseProgram(shader->program_id);
}

void layman_shader_unuse(const struct layman_shader *shader) {
	(void) shader; // Unused.
	glUseProgram(0);
}

void layman_shader_bind_uniform_material(const struct layman_shader *shader, const struct layman_material *material) {
	// TODO: These other uniforms.
	glUniform4fv(shader->uniform_base_color_factor, 1, material->base_color_factor.d);
	glUniform1i(shader->uniform_base_color_sampler, material->base_color_texture->kind);
	glUniform1i(shader->uniform_metallic_roughness_sampler, material->metallic_roughness_texture->kind);
	// TODO: float metallic_factor;
	// TODO: float roughness_factor;
	glUniform1i(shader->uniform_normal_sampler, material->normal_texture->kind);
	glUniform1i(shader->uniform_occlusion_sampler, material->occlusion_texture->kind);
	glUniform1i(shader->uniform_emissive_sampler, material->emissive_texture->kind);
	glUniform3fv(shader->uniform_emissive_factor, 1, material->emissive_factor.d);
}

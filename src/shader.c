#include "layman.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

struct layman_shader {
	GLuint program_id;
};

char *read_whole_file(const char *filepath) {
	FILE *fp = fopen(filepath, "rb");
	if (!fp) {
		return NULL;
	}

	char *content = NULL;
	size_t capacity = 0;
	size_t used = 0;

	while (!feof(fp)) {
		if (capacity - used < 1024) {
			char *new_content = realloc(content, capacity + 1024);
			if (!new_content) {
				fclose(fp);
				free(content);
				return NULL;
			}

			content = new_content;
			capacity += 1024;
		}

		size_t read = fread(content + used, 1, capacity - used, fp);
		used += read;
	}

	// Resize to final size and null terminate.

	char *new_content = realloc(content, used + 1);
	if (!new_content) {
		fclose(fp);
		free(content);
		return NULL;
	}

	content = new_content;
	content[used] = '\0';

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
GLuint compile_shader(GLenum type, const char *filepath) {
	GLuint shader_id = glCreateShader(type);
	if (shader_id == 0) {
		return 0;
	}

	char *content = read_whole_file(filepath);
	if (!content) {
		glDeleteShader(shader_id);
		return 0;
	}

	const char *const source = content;
	glShaderSource(shader_id, 1, &source, NULL);
	free(content);

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
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_POSITION, "position");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_UV, "uv");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_NORMAL, "normal");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_TANGENT, "tangent");
	glBindAttribLocation(program_id, LAYMAN_MESH_ATTRIBUTE_BITANGENT, "bitangent");

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

#define STB_IMAGE_IMPLEMENTATION
#include "layman.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <stdlib.h>

struct layman_texture {
	GLuint id;
	enum layman_texture_kind kind;
};

struct layman_texture *layman_texture_create(enum layman_texture_kind kind) {
	struct layman_texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	texture->id = 0;
	texture->kind = kind;

	return texture;
}

void layman_texture_destroy(struct layman_texture *texture) {
	free(texture);
}

// FIXME: The whole thing.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size) {
	struct layman_texture *texture = layman_texture_create(kind);
	if (!texture) {
		return NULL;
	}

	int x, y, c;
	unsigned char *rgb888 = stbi_load_from_memory(data, size, &x, &y, &c, 3); // FIXME: Error handling.

	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb888);

	// FIXME: Not always linear? What about wrapping too?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// FIXME: Mipmaps? Anisotropic filtering?
	//gl.GenerateMipmap(gl.TEXTURE_2D)
	//gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR)

	layman_texture_use(texture); // FIXME

	// FIXME: Dirty hack. Uniforms should happen in shader/renderer.
	GLint currentProgram = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	GLint location = glGetUniformLocation(currentProgram, "albedo_map");
	glUniform1i(location, texture->kind);

	free(rgb888);

	return texture;
}

void layman_texture_use(const struct layman_texture *texture) {
	// This is actually the recommended way to enumerate that constant.
	// You use the texture unit 0 and add your offset to it.
	glActiveTexture(GL_TEXTURE0 + texture->kind);
	glBindTexture(GL_TEXTURE_2D, texture->id);
}

void layman_texture_unuse(const struct layman_texture *texture) {
	glActiveTexture(GL_TEXTURE0 + texture->kind);
	glBindTexture(GL_TEXTURE_2D, 0);
}
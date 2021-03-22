#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "texture.h"
#include "utils.h"
#include <stdlib.h>

struct texture *texture_create(enum texture_kind kind, size_t width, size_t height, bool mipmapping, enum texture_type type, enum texture_format format, enum texture_format_internal format_internal) {
	struct texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	texture->gl_id = 0;
	texture->kind = kind;
	texture->width = width;
	texture->height = height;
	texture->levels = 1;

	// Automatic levels when mipmappign is enabled.
	if (mipmapping) {
		while ((width | height) >> texture->levels) {
			texture->levels++;
		}
	}

	// Figure out what OpenGL texture unit a given texture should be assigned to (based on its kind).
	// This is actually the recommended way to enumerate that constant.
	// You use the texture unit 0 and add your offset to it.
	texture->gl_unit = GL_TEXTURE0 + texture->kind;

	// Figure out what OpenGL target to use based on the kind.
	switch (kind) {
	    case TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN:
	    case TEXTURE_KIND_ENVIRONMENT_GGX:
	    case TEXTURE_KIND_ENVIRONMENT_CHARLIE:
	    case TEXTURE_KIND_CUBEMAP:
		    texture->gl_target = GL_TEXTURE_CUBE_MAP;
		    break;
	    default:
		    texture->gl_target = GL_TEXTURE_2D;
		    break;
	}

	// Translate our data type to OpenGL data type.
	switch (type) {
	    case TEXTURE_TYPE_FLOAT: texture->gl_type = GL_FLOAT; break;
	    case TEXTURE_TYPE_UNSIGNED_BYTE: texture->gl_type = GL_UNSIGNED_BYTE; break;
	}

	// Translate our data formats to OpenGL data formats.
	switch (format) {
	    case TEXTURE_FORMAT_RGB: texture->gl_format = GL_RGB; break;
	    case TEXTURE_FORMAT_RGBA: texture->gl_format = GL_RGBA; break;
	}

	// Translate our internal formats to OpenGL internal formats.
	switch (format_internal) {
	    case TEXTURE_FORMAT_INTERNAL_RGB: texture->gl_internal_format = GL_RGB; break;
	    case TEXTURE_FORMAT_INTERNAL_RGBA: texture->gl_internal_format = GL_RGBA; break;
	    case TEXTURE_FORMAT_INTERNAL_RGB8: texture->gl_internal_format = GL_RGB8; break;
	    case TEXTURE_FORMAT_INTERNAL_RGBA8: texture->gl_internal_format = GL_RGBA8; break;
	    case TEXTURE_FORMAT_INTERNAL_RGB16F: texture->gl_internal_format = GL_RGB16F; break;
	    case TEXTURE_FORMAT_INTERNAL_RGBA16F: texture->gl_internal_format = GL_RGBA16F; break;
	    case TEXTURE_FORMAT_INTERNAL_RGB32F: texture->gl_internal_format = GL_RGB32F; break;
	    case TEXTURE_FORMAT_INTERNAL_RGBA32F: texture->gl_internal_format = GL_RGBA32F; break;
	}

	glGenTextures(1, &texture->gl_id);

	texture_switch(texture);

	// Pre-allocate the storage for the pixel data.
	texture_provide_data(texture, 0, width, height, NULL);

	// Wrapping.
	// glTexParameteri(texture->gl_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(texture->gl_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering.
	// `NEAREST` is generally faster than `LINEAR`, but it can produce textured images with sharper edges
	// because the transition between texture elements is not as smooth.
	glTexParameteri(texture->gl_target, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(texture->gl_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Anisotropic filtering.
	// This is plenty high and will get capped on systems that don't support it.
	texture_anisotropic_filtering(texture, 16);

	return texture;
}

void texture_destroy(struct texture *texture) {
	if (!texture) {
		return;
	}

	glDeleteTextures(1, &texture->gl_id);
	free(texture);
}

struct texture *texture_create_from_file(enum texture_kind kind, const char *filepath) {
	if (kind == TEXTURE_KIND_EQUIRECTANGULAR) {
		// Equirectangular things are always flipped down for some reason.
		stbi_set_flip_vertically_on_load(true);

		int width, height, components;
		float *data = stbi_loadf(filepath, &width, &height, &components, 0);

		// We have to unconditionally revert that setting back, otherwise stbi will incorrectly flip future images when they get loaded.
		stbi_set_flip_vertically_on_load(false);

		if (!data) {
			return NULL;
		}

		struct texture *texture = texture_create(TEXTURE_KIND_EQUIRECTANGULAR, width, height, false, TEXTURE_TYPE_FLOAT, TEXTURE_FORMAT_RGB, TEXTURE_FORMAT_INTERNAL_RGB16F);
		if (!texture) {
			stbi_image_free(data);
			return NULL;
		}

		texture_provide_data(texture, 0, width, height, data);

		stbi_image_free(data);

		return texture;
	}

	// Texture kind not supported yet.
	return NULL;
}

struct texture *texture_create_from_memory(enum texture_kind kind, const unsigned char *data, size_t size) {
	int width, height, components;

	unsigned char *decoded = stbi_load_from_memory(data, size, &width, &height, &components, 0);
	if (!decoded) {
		return NULL;
	}

	enum texture_format format;
	switch (components) {
	    case 3: format = TEXTURE_FORMAT_RGB; break;
	    case 4: format = TEXTURE_FORMAT_RGBA; break;
	    default:
		    fprintf(stderr, "Unsupported texture data format\n");
		    stbi_image_free(decoded);
		    return NULL;
	}

	struct texture *texture = texture_create(kind, width, height, true, TEXTURE_TYPE_UNSIGNED_BYTE, format, TEXTURE_FORMAT_INTERNAL_RGBA8);
	if (!texture) {
		stbi_image_free(decoded);
		return NULL;
	}

	texture_provide_data(texture, 0, width, height, decoded);

	// Cleanup.
	stbi_image_free(decoded);

	return texture;
}

void texture_switch(const struct texture *new) {
	thread_local static const struct texture *current;

	if (current == new) {
		return;
	}

	current = new;

	if (new) {
		glActiveTexture(new->gl_unit);
		glBindTexture(new->gl_target, new->gl_id);
	}
}

void texture_provide_data(struct texture *texture, unsigned int level, unsigned int width, unsigned int height, const void *data) {
	texture_switch(texture);

	glTexImage2D(texture->gl_target, level, texture->gl_internal_format, width, height, 0, texture->gl_format, texture->gl_type, data);

	// Mimapping.
	if (level == 0 && texture->levels > 1) {
		glGenerateMipmap(texture->gl_target);
	}
}

void texture_anisotropic_filtering(struct texture *texture, float anisotropy) {
	// Ensure the driver supports the anisotropic extension before we attempt to do anything.
	if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic") == GLFW_FALSE) {
		return;
	}

	texture_switch(texture);

	// Ask OpenGL what is the maximum anisotropy we can use.
	GLfloat max_anisotropy = 1; // Fallback in case the call fails.
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);

	if (anisotropy <= max_anisotropy) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
	}
}

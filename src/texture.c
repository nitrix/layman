#define STB_IMAGE_IMPLEMENTATION
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "layman.h"
#include "stb_image.h"
#include <math.h>
#include <stdlib.h>

_Thread_local const struct layman_texture *current_texture;

static int max(int a, int b) {
	if (a > b) {
		return a;
	}

	if (b > a) {
		return b;
	}

	return a;
}

static GLenum from_kind(enum layman_texture_kind kind) {
	switch (kind) {
	    case LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR:
		    return GL_TEXTURE_2D;

	    default:
		    return GL_TEXTURE_2D;
	}
}

static GLenum from_data_format(enum layman_texture_data_format data_format) {
	switch (data_format) {
	    case LAYMAN_TEXTURE_DATA_FORMAT_RGB: return GL_RGB;

	    case LAYMAN_TEXTURE_DATA_FORMAT_RGBA: return GL_RGBA;
	}
}

static GLenum from_data_internal_format(enum layman_texture_data_internal_format data_internal_format) {
	switch (data_internal_format) {
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB: return GL_RGB;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA: return GL_RGBA;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB8: return GL_RGB8;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA8: return GL_RGBA8;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F: return GL_RGB16F;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA16F: return GL_RGBA16F;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB32F: return GL_RGB32F;

	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA32F: return GL_RGBA32F;
	}
}

static GLenum from_data_type(enum layman_texture_data_type data_type) {
	switch (data_type) {
	    case LAYMAN_TEXTURE_DATA_TYPE_FLOAT: return GL_FLOAT;

	    case LAYMAN_TEXTURE_DATA_TYPE_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	}
}

struct layman_texture *layman_texture_create(enum layman_texture_kind kind, size_t width, size_t height, size_t levels, enum layman_texture_data_type data_type, enum layman_texture_data_format data_format, enum layman_texture_data_internal_format data_internal_format) {
	struct layman_texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	texture->id = 0;
	texture->kind = kind;
	texture->width = width;
	texture->height = height;
	texture->levels = levels;
	texture->data_type = data_type;
	texture->data_format = data_format;
	texture->data_internal_format = data_internal_format;

	// Automatic levels when none were provided.
	if (levels == 0) {
		levels = 1;

		while ((width | height) >> levels) {
			levels++;
		}

		texture->levels = levels;
	}

	// TODO: Use glCreateTextures instead?
	glGenTextures(1, &texture->id);

	layman_texture_switch(texture);

	for (size_t i = 0; i < levels; i++) {
		glTexImage2D(
		        from_kind(kind),
		        i,
		        from_data_internal_format(data_internal_format),
		        width,
		        height,
		        0,
		        from_data_format(data_format),
		        from_data_type(data_type),
		        NULL);
		width = max(1, (width / 2));
	}

	glTexParameteri(from_kind(kind), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(from_kind(kind), GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(from_kind(kind), GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(from_kind(kind), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

void layman_texture_destroy(struct layman_texture *texture) {
	if (!texture) {
		return;
	}

	glDeleteTextures(1, &texture->id);
	free(texture);
}

enum layman_texture_kind layman_texture_kind(const struct layman_texture *texture) {
	return texture->kind;
}

struct layman_texture *layman_texture_create_from_file(enum layman_texture_kind kind, const char *filepath) {
	// TODO: if extension is .hdr
	if (kind == LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR) {
		// Equirectangular things are always flipped down for some reason.
		stbi_set_flip_vertically_on_load(true);

		int w, h, c;
		float *data = stbi_loadf(filepath, &w, &h, &c, 0);
		if (!data) {
			// Important! Once we're done, we have to revert that setting back, otherwise stbi will
			// incorrectly flip future images when they get loaded.
			stbi_set_flip_vertically_on_load(false);
			return NULL;
		}

		// Important! Once we're done, we have to revert that setting back, otherwise stbi will
		// incorrectly flip future images when they get loaded.
		stbi_set_flip_vertically_on_load(false);

		struct layman_texture *texture = layman_texture_create(LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR, w, h, 1, LAYMAN_TEXTURE_DATA_TYPE_FLOAT, LAYMAN_TEXTURE_DATA_FORMAT_RGB, LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F);
		if (!texture) {
			stbi_image_free(data);
			return NULL;
		}

		layman_texture_provide_data(texture, data);

		stbi_image_free(data);

		// TODO: Mimapping?
		// TODO: Anisotropic filtering?
		// TODO: All those other things... should that be shared in common with layman_texture_create_from_memory?

		return texture;
	}

	return NULL;
}

// FIXME: The whole thing.
// TODO: Should have a create_from_file also.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size) {
	int w, h, c;

	unsigned char *decoded = stbi_load_from_memory(data, size, &w, &h, &c, 0);
	if (!decoded) {
		return NULL;
	}

	enum layman_texture_data_format data_format;
	switch (c) {
	    case 3: {
		    data_format = LAYMAN_TEXTURE_DATA_FORMAT_RGB;
		    break;
	    }

	    case 4: {
		    data_format = LAYMAN_TEXTURE_DATA_FORMAT_RGBA;
		    break;
	    }

	    default:
		    fprintf(stderr, "Unsupported texture data format\n");
		    stbi_image_free(decoded);
		    return NULL;
	}

	struct layman_texture *texture = layman_texture_create(kind, w, h, 0, LAYMAN_TEXTURE_DATA_TYPE_UNSIGNED_BYTE, data_format, LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA8);
	if (!texture) {
		stbi_image_free(decoded);
		return NULL;
	}

	layman_texture_provide_data(texture, decoded);

	layman_texture_switch(texture);

	// Mimapping.
	if (texture->levels > 1) {
		glGenerateMipmap(from_kind(kind));
	}

	// Anisotropic filtering.
	// This is plenty high and will get capped on systems that don't support it.
	layman_texture_anisotropic_filtering(texture, 16);

	// Cleanup.
	stbi_image_free(decoded);

	return texture;
}

void layman_texture_switch(const struct layman_texture *texture) {
	if (current_texture == texture) {
		return;
	} else {
		current_texture = texture;
	}

	printf("Texture switch: %p\n", texture);

	// This is actually the recommended way to enumerate that constant.
	// You use the texture unit 0 and add your offset to it.
	glActiveTexture(GL_TEXTURE0 + texture->kind);
	glBindTexture(from_kind(texture->kind), texture->id);
}

// FIXME: Could save these format things in the texture type and not bother with it here.
void layman_texture_provide_data(struct layman_texture *texture, const void *data) {
	layman_texture_switch(texture);

	glTexImage2D(
	        from_kind(texture->kind),
	        0,
	        from_data_internal_format(texture->data_internal_format),
	        texture->width,
	        texture->height,
	        0,
	        from_data_format(texture->data_format),
	        from_data_type(texture->data_type),
	        data);
}

void layman_texture_anisotropic_filtering(struct layman_texture *texture, float anisotropy) {
	// Ensure the driver supports the anisotropic extension before we attempt to do anything.
	if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic") == GLFW_FALSE) {
		return;
	}

	layman_texture_switch(texture);

	// Ask OpenGL what is the maximum anisotropy we can use.
	GLfloat max_anisotropy = 1.0; // Fallback in case the call fails.
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);

	if (anisotropy <= max_anisotropy) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
	}
}

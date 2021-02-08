#include "layman.h"

struct layman_texture *layman_texture_create(enum layman_texture_kind kind, size_t width, size_t height, size_t levels, enum layman_texture_data_type data_type, enum layman_texture_data_format data_format, enum layman_texture_data_internal_format data_internal_format) {
	struct layman_texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	texture->gl_id = 0;
	texture->kind = kind;
	texture->width = width;
	texture->height = height;

	// Automatic levels when none were provided.
	if (levels == 0) {
		levels = 1;

		while ((width | height) >> levels) {
			levels++;
		}
	}

	texture->levels = levels;

	// Figure out what OpenGL texture unit a given texture should be assigned to (based on its kind).
	// This is actually the recommended way to enumerate that constant.
	// You use the texture unit 0 and add your offset to it.
	texture->gl_unit = GL_TEXTURE0 + texture->kind;

	// Figure out what OpenGL target to use based on the kind.
	switch (kind) {
	    case LAYMAN_TEXTURE_KIND_CUBEMAP: texture->gl_target = GL_TEXTURE_CUBE_MAP; break;
	    default: texture->gl_target = GL_TEXTURE_2D; break;
	}

	// Translate our data type to OpenGL data type.
	switch (data_type) {
	    case LAYMAN_TEXTURE_DATA_TYPE_FLOAT: texture->gl_type = GL_FLOAT; break;
	    case LAYMAN_TEXTURE_DATA_TYPE_UNSIGNED_BYTE: texture->gl_type = GL_UNSIGNED_BYTE; break;
	}

	// Translate our data formats to OpenGL data formats.
	switch (data_format) {
	    case LAYMAN_TEXTURE_DATA_FORMAT_RGB: texture->gl_format = GL_RGB; break;
	    case LAYMAN_TEXTURE_DATA_FORMAT_RGBA: texture->gl_format = GL_RGBA; break;
	}

	// Translate our data internal formats to OpenGL data internal formats.
	switch (data_internal_format) {
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB: texture->gl_internal_format = GL_RGB; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA: texture->gl_internal_format = GL_RGBA; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB8: texture->gl_internal_format = GL_RGB8; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA8: texture->gl_internal_format = GL_RGBA8; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F: texture->gl_internal_format = GL_RGB16F; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA16F: texture->gl_internal_format = GL_RGBA16F; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB32F: texture->gl_internal_format = GL_RGB32F; break;
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA32F: texture->gl_internal_format = GL_RGBA32F; break;
	}

	glGenTextures(1, &texture->gl_id);

	layman_texture_switch(texture);

	// Pre-allocate the storage for the pixel data.
	for (size_t i = 0; i < levels; i++) {
		layman_texture_provide_data(texture, i, NULL);
		width = MAX(1, (width / 2));
	}

	// Wrapping.
	glTexParameteri(texture->gl_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture->gl_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering.
	glTexParameteri(texture->gl_target, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(texture->gl_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Anisotropic filtering.
	// This is plenty high and will get capped on systems that don't support it.
	layman_texture_anisotropic_filtering(texture, 16);

	return texture;
}

void layman_texture_destroy(struct layman_texture *texture) {
	if (!texture) {
		return;
	}

	glDeleteTextures(1, &texture->gl_id);
	free(texture);
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

		// TODO: Not sure if these are mendatory.
		layman_texture_switch(texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		layman_texture_provide_data(texture, 0, data);

		stbi_image_free(data);

		return texture;
	}

	return NULL;
}

struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size) {
	int width, height, components;

	unsigned char *decoded = stbi_load_from_memory(data, size, &width, &height, &components, 0);
	if (!decoded) {
		return NULL;
	}

	enum layman_texture_data_format data_format;
	switch (components) {
	    case 3: data_format = LAYMAN_TEXTURE_DATA_FORMAT_RGB; break;
	    case 4: data_format = LAYMAN_TEXTURE_DATA_FORMAT_RGBA; break;
	    default:
		    fprintf(stderr, "Unsupported texture data format\n");
		    stbi_image_free(decoded);
		    return NULL;
	}

	struct layman_texture *texture = layman_texture_create(kind, width, height, 0, LAYMAN_TEXTURE_DATA_TYPE_UNSIGNED_BYTE, data_format, LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGBA8);
	if (!texture) {
		stbi_image_free(decoded);
		return NULL;
	}

	layman_texture_provide_data(texture, 0, decoded);

	// Cleanup.
	stbi_image_free(decoded);

	return texture;
}

void layman_texture_switch(const struct layman_texture *new) {
	thread_local static const struct layman_texture *current;

	if (current == new) {
		return;
	}

	current = new;

	if (new) {
		glActiveTexture(new->gl_unit);
		glBindTexture(new->gl_target, new->gl_id);
	}
}

void layman_texture_provide_data(struct layman_texture *texture, size_t level, const void *data) {
	layman_texture_switch(texture);

	glTexImage2D(
	        texture->gl_target,
	        level,
	        texture->gl_internal_format,
	        texture->width,
	        texture->height,
	        0, // Always zero.
	        texture->gl_format,
	        texture->gl_type,
	        data);

	// Mimapping.
	if (texture->levels > 1) {
		glGenerateMipmap(texture->gl_target);
	}
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

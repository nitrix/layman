#define STB_IMAGE_IMPLEMENTATION
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "layman.h"
#include "stb_image.h"
#include <stdlib.h>

struct layman_texture *layman_texture_create(enum layman_texture_kind kind) {
	struct layman_texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	texture->id = 0;
	texture->kind = kind;

	glGenTextures(1, &texture->id);

	struct layman_texture previous_texture;
	layman_texture_switch(texture, &previous_texture);

	// Default wrap behavior and filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	layman_texture_switch(&previous_texture, NULL);

	return texture;
}

void layman_texture_destroy(struct layman_texture *texture) {
	glDeleteTextures(1, &texture->id);
	free(texture);
}

enum layman_texture_kind layman_texture_kind(const struct layman_texture *texture) {
	return texture->kind;
}

// FIXME: The whole thing.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size) {
	struct layman_texture *texture = layman_texture_create(kind);
	if (!texture) {
		return NULL;
	}

	int w, h, c;
	unsigned char *image = stbi_load_from_memory(data, size, &w, &h, &c, 0); // FIXME: Error handling.

	struct layman_texture previous_texture;
	layman_texture_switch(texture, &previous_texture);

	// FIXME: Not always linear? What about wrapping too?
	// bool generate_mipmaps = false; // TODO: Implement mipmaps and texture wrapping.
	// if (generate_mipmaps) {
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	// } else {
	// The glTF documentation doesn't specify what to use when samplers aren't specified.
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// }

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLenum format;
	switch (c) {
	    /*
	       case 1: {
	       // Gray
	       format = GL_LUMINANCE;
	       break;
	       }
	       case 2: {
	       // Gray and Alpha
	       format = GL_LUMINANCE_ALPHA;
	       break;
	       }
	     */
	    case 3: {
		    // RGB
		    format = GL_RGB;
		    break;
	    }

	    case 4: {
		    // RGBA
		    format = GL_RGBA;
		    break;
	    }
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	layman_texture_switch(&previous_texture, NULL);

	// If not mip-mapped, force to non-mip-mapped sampler.
	/*
	   if (!generateMipmaps && (gltfSamplerObj.minFilter != WebGl.context.NEAREST) && (gltfSamplerObj.minFilter != WebGl.context.LINEAR))
	   {
	        if ((gltfSamplerObj.minFilter == WebGl.context.NEAREST_MIPMAP_NEAREST) || (gltfSamplerObj.minFilter == WebGl.context.NEAREST_MIPMAP_LINEAR))
	        {
	                WebGl.context.texParameteri(type, WebGl.context.TEXTURE_MIN_FILTER, WebGl.context.NEAREST);
	        }
	        else
	        {
	                WebGl.context.texParameteri(type, WebGl.context.TEXTURE_MIN_FILTER, WebGl.context.LINEAR);
	        }
	   }
	   else
	   {
	        WebGl.context.texParameteri(type, WebGl.context.TEXTURE_MIN_FILTER, gltfSamplerObj.minFilter);
	   }
	   WebGl.context.texParameteri(type, WebGl.context.TEXTURE_MAG_FILTER, gltfSamplerObj.magFilter);
	 */

	// Mipmaping.
	// glGenerateMipmap(GL_TEXTURE_2D);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Anisotropic filtering.
	// This is plenty high and will get capped on systems that don't support it.
	// layman_texture_anisotropic_filtering(texture, 16);

	// TODO: Is it normal that the texture remains in use?
	// layman_texture_use(texture); // FIXME

	// TODO: Should we free this?
	stbi_image_free(image);

	return texture;
}

void layman_texture_switch(const struct layman_texture *new_texture, struct layman_texture *old_texture) {
	if (old_texture) {
		GLint unit;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &unit);
		old_texture->kind = unit;
		GLint id;
		glGetIntegerv(GL_TEXTURE_2D, &id);
		old_texture->id = id;
	}

	if (new_texture) {
		// This is actually the recommended way to enumerate that constant.
		// You use the texture unit 0 and add your offset to it.
		glActiveTexture(GL_TEXTURE0 + new_texture->kind);
		glBindTexture(GL_TEXTURE_2D, new_texture->id);
	}
}

void layman_texture_provide_data(struct layman_texture *texture, void *data, int width, int height, enum layman_texture_data_type data_type, enum layman_texture_data_format data_format, enum layman_texture_data_internal_format data_internal_format) {
	struct layman_texture previous_texture;
	layman_texture_switch(texture, &previous_texture);

	GLenum gl_data_type;
	switch (data_type) {
	    case LAYMAN_TEXTURE_DATA_TYPE_FLOAT:
		    gl_data_type = GL_RGB16F;
		    break;
	}

	GLenum gl_data_format;
	switch (data_format) {
	    case LAYMAN_TEXTURE_DATA_FORMAT_RGB:
		    gl_data_format = GL_RGB;
		    break;

	    case LAYMAN_TEXTURE_DATA_FORMAT_RGBA:
		    gl_data_format = GL_RGBA;
		    break;
	}

	GLenum gl_data_internal_format;
	switch (data_internal_format) {
	    case LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F:
		    gl_data_internal_format = GL_RGB16F;
		    break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, gl_data_internal_format, width, height, 0, gl_data_format, gl_data_type, data);

	layman_texture_switch(&previous_texture, NULL);
}

void layman_texture_anisotropic_filtering(struct layman_texture *texture, float anisotropy) {
	// Ensure the driver supports the anisotropic extension before we attempt to do anything.
	if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic") == GLFW_FALSE) {
		return;
	}

	// Temporarily switch texture and save the previous one.
	struct layman_texture previous_texture;
	layman_texture_switch(texture, &previous_texture);

	// Ask OpenGL what is the maximum anisotropy we can use.
	GLfloat max_anisotropy = 1.0; // Fallback in case the call fails.
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);

	if (anisotropy <= max_anisotropy) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
	}

	// Restore previous texture.
	layman_texture_switch(&previous_texture, NULL);
}

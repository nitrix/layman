#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"
#include <stdbool.h>
#include <stdlib.h>

enum texture_kind {
	// PBR textures.
	TEXTURE_KIND_ALBEDO,
	TEXTURE_KIND_NORMAL,
	TEXTURE_KIND_METALLIC_ROUGHNESS,
	TEXTURE_KIND_OCCLUSION,
	TEXTURE_KIND_EMISSION,

	// IBL environment textures.
	TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN,
	TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN_LUT,
	TEXTURE_KIND_ENVIRONMENT_GGX,
	TEXTURE_KIND_ENVIRONMENT_GGX_LUT,
	TEXTURE_KIND_ENVIRONMENT_CHARLIE,
	TEXTURE_KIND_ENVIRONMENT_CHARLIE_LUT,

	// Other things.
	TEXTURE_KIND_IMAGE,
	TEXTURE_KIND_EQUIRECTANGULAR,
	TEXTURE_KIND_CUBEMAP,

	// Keep there.
	TEXTURE_KIND_COUNT,
};

enum texture_type {
	TEXTURE_TYPE_UNSIGNED_BYTE,
	TEXTURE_TYPE_FLOAT
};

enum texture_format {
	TEXTURE_FORMAT_RGB,
	TEXTURE_FORMAT_RGBA,
};

enum texture_format_internal {
	TEXTURE_FORMAT_INTERNAL_RGB,
	TEXTURE_FORMAT_INTERNAL_RGB8,
	TEXTURE_FORMAT_INTERNAL_RGB16F,
	TEXTURE_FORMAT_INTERNAL_RGB32F,
	TEXTURE_FORMAT_INTERNAL_RGBA,
	TEXTURE_FORMAT_INTERNAL_RGBA8,
	TEXTURE_FORMAT_INTERNAL_RGBA16F,
	TEXTURE_FORMAT_INTERNAL_RGBA32F,
};

struct texture {
	size_t width;
	size_t height;
	size_t levels;

	enum texture_kind kind;

	// Object stuff.
	GLuint gl_id;

	// Sampler stuff.
	GLenum gl_target;
	GLenum gl_unit;

	// Pixel data stuff.
	GLenum gl_type;
	GLenum gl_format;
	GLenum gl_internal_format;
};

void texture_switch(const struct texture *texture);

// TODO: Documentation.
struct texture *texture_create(enum texture_kind kind, size_t width, size_t height, bool mipmapping, enum texture_type type, enum texture_format format, enum texture_format_internal format_internal);
struct texture *texture_create_from_file(enum texture_kind kind, const char *filepath);
struct texture *texture_create_from_memory(enum texture_kind kind, const unsigned char *data, size_t size);
void texture_destroy(struct texture *texture);
void texture_provide_data(struct texture *texture, unsigned int level, unsigned int width, unsigned int height, const void *data);
void texture_anisotropic_filtering(struct texture *texture, float anisotropy);

#endif

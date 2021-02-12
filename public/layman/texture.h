#ifndef LAYMAN_TEXTURE_H
#define LAYMAN_TEXTURE_H

#include <stdlib.h>

enum layman_texture_kind {
	// PBR textures.
	LAYMAN_TEXTURE_KIND_ALBEDO,
	LAYMAN_TEXTURE_KIND_NORMAL,
	LAYMAN_TEXTURE_KIND_METALLIC_ROUGHNESS,
	LAYMAN_TEXTURE_KIND_OCCLUSION,
	LAYMAN_TEXTURE_KIND_EMISSION,

	// IBL environment textures.
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN,
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN_LUT,
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_GGX,
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_GGX_LUT,
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_CHARLIE,
	LAYMAN_TEXTURE_KIND_ENVIRONMENT_CHARLIE_LUT,

	// Other things.
	LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR,
	LAYMAN_TEXTURE_KIND_CUBEMAP,

	// Keep there.
	LAYMAN_TEXTURE_KIND_COUNT,
};

enum layman_texture_type {
	LAYMAN_TEXTURE_TYPE_UNSIGNED_BYTE,
	LAYMAN_TEXTURE_TYPE_FLOAT
};

enum layman_texture_format {
	LAYMAN_TEXTURE_FORMAT_RGB,
	LAYMAN_TEXTURE_FORMAT_RGBA,
};

enum layman_texture_format_internal {
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGB,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGB8,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGB16F,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGB32F,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGBA,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGBA8,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGBA16F,
	LAYMAN_TEXTURE_FORMAT_INTERNAL_RGBA32F,
};

// TODO: Documentation.
struct layman_texture *layman_texture_create(enum layman_texture_kind kind, size_t width, size_t height, bool mipmapping, enum layman_texture_type type, enum layman_texture_format format, enum layman_texture_format_internal format_internal);

// TODO: Documentation.
struct layman_texture *layman_texture_create_from_file(enum layman_texture_kind kind, const char *filepath);

// TODO: Documentation.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size);

// TODO: Documentation.
void layman_texture_destroy(struct layman_texture *texture);

// TODO: Documentation.
void layman_texture_provide_data(struct layman_texture *texture, size_t level, size_t width, size_t height, const void *data);

// TODO: Documentation.
void layman_texture_anisotropic_filtering(struct layman_texture *texture, float anisotropy);

#endif

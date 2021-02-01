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

	// Keep there.
	LAYMAN_TEXTURE_KIND_COUNT,
};

enum layman_texture_data_type {
	LAYMAN_TEXTURE_DATA_TYPE_FLOAT
};

enum layman_texture_data_format {
	LAYMAN_TEXTURE_DATA_FORMAT_RGB,
	LAYMAN_TEXTURE_DATA_FORMAT_RGBA,
};

enum layman_texture_data_internal_format {
	LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F,
};

// TODO: Documentation.
struct layman_texture *layman_texture_create(enum layman_texture_kind kind);

// TODO: Documentation.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size);

// TODO: Documentation.
enum layman_texture_kind layman_texture_kind(const struct layman_texture *texture);

// TODO: Documentation.
void layman_texture_destroy(struct layman_texture *texture);

// TODO: Documentation.
void layman_texture_anisotropic_filtering(struct layman_texture *texture, float anisotropy);

// TODO: Documentation.
void layman_texture_switch(const struct layman_texture *new_texture, struct layman_texture *old_texture);

// TODO: Documentation.
void layman_texture_provide_data(struct layman_texture *texture, void *data, int width, int height, enum layman_texture_data_type data_type, enum layman_texture_data_format data_format, enum layman_texture_data_internal_format data_internal_format);

#endif

#ifndef LAYMAN_TEXTURE_H
#define LAYMAN_TEXTURE_H

#include <stdlib.h>

enum layman_texture_kind {
	LAYMAN_TEXTURE_KIND_ALBEDO,
	LAYMAN_TEXTURE_KIND_NORMAL,
	LAYMAN_TEXTURE_KIND_METALLIC_ROUGHNESS,
	LAYMAN_TEXTURE_KIND_OCCLUSION,
	LAYMAN_TEXTURE_KIND_EMISSION,

	LAYMAN_TEXTURE_KIND_COUNT,
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

#endif

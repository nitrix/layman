#ifndef LAYMAN_TEXTURE_H
#define LAYMAN_TEXTURE_H

#include <stdlib.h>

enum layman_texture_kind {
	LAYMAN_TEXTURE_ALBEDO,
	LAYMAN_TEXTURE_NORMAL,
	LAYMAN_TEXTURE_ROUGHNESS_METALLIC,
	LAYMAN_TEXTURE_OCCLUSION,
	LAYMAN_TEXTURE_EMISSION,
};

// TODO: Documentation.
struct layman_texture *layman_texture_create(enum layman_texture_kind kind);

// TODO: Documentation.
struct layman_texture *layman_texture_create_from_memory(enum layman_texture_kind kind, const unsigned char *data, size_t size);

// TODO: Documentation.
void layman_texture_destroy(struct layman_texture *texture);

// TODO: Documentation.
void layman_texture_anisotropic_filtering(struct layman_texture *texture, float anisotropy);

// TODO: Documentation.
void layman_texture_switch(const struct layman_texture *new_texture, struct layman_texture *old_texture);

#endif

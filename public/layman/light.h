#ifndef LAYMAN_PUBLIC_LIGHT_H
#define LAYMAN_PUBLIC_LIGHT_H

// Passed as-is to the shaders and must therefore match the enums in them.
enum layman_light_type {
	LAYMAN_LIGHT_TYPE_DIRECTIONAL = 0,
	LAYMAN_LIGHT_TYPE_POINT = 1,
	LAYMAN_LIGHT_TYPE_SPOT = 2,
};

struct layman_light *layman_light_create(enum layman_light_type type);
void layman_light_destroy(struct layman_light *light);

#endif

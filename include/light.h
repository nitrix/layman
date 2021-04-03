#ifndef LIGHT_H
#define LIGHT_H

#include "cglm/cglm.h"

// Passed as-is to the shaders and must therefore match the enums in them.
enum light_type {
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_POINT = 1,
	LIGHT_TYPE_SPOT = 2,
};

struct light {
	enum light_type type;
	vec3 position;
	vec3 direction;
	float range;
	vec3 color;
	float intensity;
	float innerConeCos;
	float outerConeCos;
};

#define MAX_LIGHTS 4

void light_init(struct light *light, enum light_type type);

#endif

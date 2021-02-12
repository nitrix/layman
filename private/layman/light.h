#ifndef LAYMAN_PRIVATE_LIGHT_H
#define LAYMAN_PRIVATE_LIGHT_H

struct layman_light {
	enum layman_light_type type;
	vec3 position;
	vec3 direction;
	float range;
	vec3 color;
	float intensity;
	float innerConeCos;
	float outerConeCos;
};

#endif

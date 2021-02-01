#ifndef LAYMAN_PRIVATE_LIGHT_H
#define LAYMAN_PRIVATE_LIGHT_H

struct layman_light {
	enum layman_light_type type;
	struct layman_vector_3f position;
	struct layman_vector_3f direction;
	float range;
	struct layman_vector_3f color;
	float intensity;
	float innerConeCos;
	float outerConeCos;
};

#endif

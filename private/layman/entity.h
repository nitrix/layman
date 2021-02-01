#ifndef LAYMAN_PRIVATE_ENTITY_H
#define LAYMAN_PRIVATE_ENTITY_H

struct layman_entity {
	const struct layman_model *model;
	struct layman_vector_3f position;
};

#endif

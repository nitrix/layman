#ifndef LAYMAN_PRIVATE_MODEL_H
#define LAYMAN_PRIVATE_MODEL_H

#include <stdlib.h>

struct layman_model {
	struct layman_mesh **meshes;
	size_t meshes_count;
};

#endif

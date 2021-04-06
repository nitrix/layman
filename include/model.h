#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>

struct model {
	char *filepath;
	struct mesh *meshes;
	size_t meshes_count;
};

struct model *model_load(const char *filepath);
void model_destroy(struct model *model);

#endif

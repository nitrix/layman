#ifndef MODEL_H
#define MODEL_H

#include "window.h"

struct model {
	char *name;
	struct mesh **meshes;
	size_t meshes_count;
};

struct model *model_load(const char *filepath);
void model_destroy(struct model *model);
void model_render(const struct model *model);

#endif

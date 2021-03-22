#ifndef MODEL_H
#define MODEL_H

#include "window.h"

struct model {
	struct mesh **meshes;
	size_t meshes_count;
};

// TODO: Documentation.
struct model *model_load(const struct window *window, const char *filepath);

// TODO: Documentation.
void model_destroy(struct model *model);

// TODO: Documentation.
void layman_model_render(const struct model *model);

#endif

#ifndef LAYMAN_PUBLIC_MODEL_H
#define LAYMAN_PUBLIC_MODEL_H

#include "window.h"

// TODO: Documentation.
struct layman_model *layman_model_load(const struct layman_window *window, const char *filepath);

// TODO: Documentation.
void layman_model_destroy(struct layman_model *model);

// TODO: Documentation.
void layman_model_render(const struct layman_model *model);

#endif

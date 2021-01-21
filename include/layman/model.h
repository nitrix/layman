#ifndef LAYMAN_MODEL_H
#define LAYMAN_MODEL_H

// TODO: Documentation.
struct layman_model *layman_model_load(const char *filepath);

// TODO: Documentation.
void layman_model_destroy(struct layman_model *model);

// TODO: Documentation.
void layman_model_render(const struct layman_model *model);

#endif
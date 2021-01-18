#ifndef LAYMAN_MODEL_H
#define LAYMAN_MODEL_H

struct layman_model *layman_model_load(const char *filepath);
void layman_model_destroy(struct layman_model *model);

void layman_model_render(const struct layman_model *model);

#endif
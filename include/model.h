#ifndef LEARNGL_MODEL_H
#define LEARNGL_MODEL_H

#include "toolkit.h"

struct model;

struct model *model_create_raw(float *vertices, size_t vertex_count, unsigned int *indices, size_t index_count);
void model_destroy(struct model *model);

void model_use(struct model *model);
void model_unuse(struct model *model);
size_t model_vertex_count(struct model *model);
size_t model_index_count(struct model *model);

#endif
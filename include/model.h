#ifndef LEARNGL_MODEL_H
#define LEARNGL_MODEL_H

#include "toolkit.h"

enum MODEL_BUFFER_TYPES {
    MODEL_BUFFER_VERTICES = 0,
    MODEL_BUFFER_EDGES    = 1,
    MODEL_BUFFER_COUNT    = 2,
};

struct model;

struct model *model_create_from_raw(float *vertices, size_t vertex_count, unsigned int *edges, size_t edge_count);
void model_destroy(struct model *model);

void model_use(struct model *model);
size_t model_vertex_count(struct model *model);
size_t model_face_count(struct model *model);

#endif

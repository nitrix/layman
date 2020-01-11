#ifndef LEARNGL_MODEL_H
#define LEARNGL_MODEL_H

#include "toolkit.h"

enum model_attributes {
    MODEL_ATTRIBUTE_VERTEX_COORDINATES = 0,
    MODEL_ATTRIBUTE_TEXTURE_COORDINATES = 1,
};

struct model;

struct model *model_create_from_raw(float *vertices, size_t vertex_count, unsigned int *edges, size_t edge_count, float *texture_uvs);
void model_destroy(struct model *model);

void model_use(struct model *model);
size_t model_vertex_count(struct model *model);
size_t model_face_count(struct model *model);

#endif

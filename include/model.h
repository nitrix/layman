#ifndef LEARNGL_MODEL_H
#define LEARNGL_MODEL_H

#include "toolkit.h"

enum model_attributes {
    MODEL_ATTRIBUTE_VERTEX_COORDINATES = 0,
    MODEL_ATTRIBUTE_TEXTURE_COORDINATES = 1,
    MODEL_ATTRIBUTE_NORMALS = 2,
};

struct model;

struct model *model_create_from_raw(const float *vertices, size_t vertex_count, const unsigned int *edges, size_t edge_count, const float *texture_uvs, const float *normals);
void model_destroy(struct model *model);

void model_use(const struct model *model);
size_t model_vertex_count(const struct model *model);
size_t model_face_count(const struct model *model);

#endif

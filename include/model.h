#ifndef LEARNGL_MODEL_H
#define LEARNGL_MODEL_H

#include "toolkit.h"

#include <glad/glad.h>

enum model_attributes {
    MODEL_ATTRIBUTE_VERTEX_COORDINATES = 0,
    MODEL_ATTRIBUTE_TEXTURE_COORDINATES = 1,
    MODEL_ATTRIBUTE_NORMALS = 2,
};

struct model {
    GLuint vertex_array_id;

    GLuint vertices_buffer_id;
    GLuint faces_buffer_id;
    GLuint texture_uvs_buffer_id;
    GLuint normals_buffer_id;

    size_t vertex_count;
    size_t face_count;
};

struct model *model_create_from_raw(const float *vertices, size_t vertex_count, const unsigned int *edges, size_t edge_count, const float *texture_uvs, const float *normals);
void model_destroy(struct model *model);

void model_use(const struct model *model);

#endif

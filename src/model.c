#include "toolkit.h"

#include <GL/glew.h>

enum VBO_TYPES {
    VBO_VERTICES = 0,
    VBO_INDICES  = 1,
    VBO_COUNT    = 2,
};

struct model {
    GLuint vao_id;
    GLuint vbo_ids[VBO_COUNT];
    GLsizei vertex_count;
    GLsizei index_count;
};

void model_use(struct model *model) {
    glBindVertexArray(model->vao_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vbo_ids[1]);
    glEnableVertexAttribArray(0);
}

void model_unuse(struct model *model) {
    TK_UNUSED(model);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

size_t model_vertex_count(struct model *model) {
    return model->vertex_count;
}

size_t model_index_count(struct model *model) {
    return model->index_count;
}

struct model *model_create_raw(float *vertices, size_t vertex_count, unsigned int *indices, size_t index_count) {
    struct model *model = malloc(sizeof *model);

    if (!model) {
        return NULL;
    }

    if (glGenVertexArrays == NULL) {
        fprintf(stderr, "missing function pointer\n");
    }

    // Create the VAO on the GPU.
    glGenVertexArrays(1, &model->vao_id);

    // Use the VAO we just created
    glBindVertexArray(model->vao_id);

    // Create the VBOs on the GPU.
    glGenBuffers(VBO_COUNT, model->vbo_ids);

    // Vertices VBO.
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo_ids[VBO_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof *vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Indices VBO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->vbo_ids[VBO_INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof *indices, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // No longer need the VAO.
    glBindVertexArray(0);

    // Book-keeping
    model->vertex_count = vertex_count;
    model->index_count = index_count;

    return model;
}

void model_destroy(struct model *model) {
    glDeleteBuffers(VBO_COUNT, model->vbo_ids);
    glDeleteVertexArrays(1, &model->vao_id);
    free(model);
}

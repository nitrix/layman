#include "toolkit.h"
#include "model.h"
#include <GL/glew.h>

struct model {
    GLuint vao_id;
    GLuint buffer_ids[MODEL_BUFFER_COUNT];

    size_t vertex_count;
    size_t face_count;
};

void model_use(struct model *model) {
    glBindVertexArray(model->vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, model->buffer_ids[MODEL_BUFFER_VERTICES]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->buffer_ids[MODEL_BUFFER_EDGES]);
    glEnableVertexAttribArray(MODEL_BUFFER_VERTICES);
}

size_t model_vertex_count(struct model *model) {
    return model->vertex_count;
}

size_t model_face_count(struct model *model) {
    return model->face_count;
}

struct model *model_create_from_raw(float *vertices, size_t vertex_count, unsigned int *faces, size_t face_count) {
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

    // Create the buffers (VBO and EBO) on the GPU.
    glGenBuffers(MODEL_BUFFER_COUNT, model->buffer_ids);

    // Vertex Buffer Object (VBO).
    glBindBuffer(GL_ARRAY_BUFFER, model->buffer_ids[MODEL_BUFFER_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof *vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(MODEL_BUFFER_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    // Element Buffer Object (EBO).
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->buffer_ids[MODEL_BUFFER_EDGES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * face_count * sizeof *faces, faces, GL_STATIC_DRAW);

    // No longer need the VAO.
    glBindVertexArray(0);

    // Book-keeping
    model->vertex_count = vertex_count;
    model->face_count = face_count;

    return model;
}

void model_destroy(struct model *model) {
    glDeleteBuffers(MODEL_BUFFER_COUNT, model->buffer_ids);
    glDeleteVertexArrays(1, &model->vao_id);
    free(model);
}

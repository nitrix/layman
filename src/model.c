#include "toolkit.h"
#include <GL/glew.h>

struct model {
    GLuint vao_id;
    GLuint vbo_id;
    GLsizei vertex_count;
};

void model_use(struct model *model) {
    glBindVertexArray(model->vao_id);
    glEnableVertexAttribArray(0);
}

void model_unuse(struct model *model) {
    TK_UNUSED(model);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

size_t model_vertex_count(struct model *model) {
    return model->vertex_count;
}

struct model *model_create_raw(float *vertices, size_t vertex_count) {
    struct model *model = malloc(sizeof *model);

    if (!model) {
        return NULL;
    }

    if (glGenVertexArrays == NULL) {
        fprintf(stderr, "missing function pointer\n");
    }

    // Create VAO(s) on the GPU. Only one is needed in our case.
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // Use the VAO we just created
    glBindVertexArray(vao);

    // Create the VBO(s) on the GPU. Only one is needed in our case.
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Use the VBO we just created to fill it with the vertices we got.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof *vertices, vertices, GL_STATIC_DRAW);

    // Register our VBO in our VAO as the first attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0);

    // No longer need the VAO and VBO active.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Book-keeping
    model->vao_id = vao;
    model->vbo_id = vbo;
    model->vertex_count = vertex_count;

    return model;
}

void model_destroy(struct model *model) {
    glDeleteBuffers(1, &model->vbo_id);
    glDeleteVertexArrays(1, &model->vao_id);
    free(model);
}

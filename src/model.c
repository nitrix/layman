#include "model.h"
#include "toolkit.h"

#include <GL/glew.h>

struct model {
    GLuint vertex_array_id;

    GLuint vertices_buffer_id;
    GLuint faces_buffer_id;
    GLuint texture_uvs_buffer_id;
    GLuint normals_buffer_id;

    size_t vertex_count;
    size_t face_count;
};

void model_use(struct model *model) {
    glBindVertexArray(model->vertex_array_id);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, model->vertices_buffer_id);
    glEnableVertexAttribArray(MODEL_ATTRIBUTE_VERTEX_COORDINATES);

    // Texture uvs
    glBindBuffer(GL_ARRAY_BUFFER, model->texture_uvs_buffer_id);
    glEnableVertexAttribArray(MODEL_ATTRIBUTE_TEXTURE_COORDINATES);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, model->normals_buffer_id);
    glEnableVertexAttribArray(MODEL_ATTRIBUTE_NORMALS);

    // Faces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->faces_buffer_id);
}

size_t model_vertex_count(struct model *model) {
    return model->vertex_count;
}

size_t model_face_count(struct model *model) {
    return model->face_count;
}

struct model *model_create_from_raw(float *vertices, size_t vertex_count, unsigned int *faces, size_t face_count, float *texture_uvs, float *normals) {
    struct model *model = malloc(sizeof *model);

    if (!model) {
        return NULL;
    }

    if (glGenVertexArrays == NULL) {
        fprintf(stderr, "missing function pointer\n");
    }

    // Create the VAO on the GPU, then use it.
    glGenVertexArrays(1, &model->vertex_array_id);
    glBindVertexArray(model->vertex_array_id);

    // Vertex Buffer Object (VBO).
    glGenBuffers(1, &model->vertices_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, model->vertices_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof *vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(MODEL_ATTRIBUTE_VERTEX_COORDINATES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0);

    // Texture UVs.
    glGenBuffers(1, &model->texture_uvs_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, model->texture_uvs_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 2 * vertex_count * sizeof *texture_uvs, texture_uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(MODEL_ATTRIBUTE_TEXTURE_COORDINATES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), 0);

    // Texture UVs.
    glGenBuffers(1, &model->normals_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, model->normals_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof *normals, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(MODEL_ATTRIBUTE_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0);

    // Element Buffer Object (EBO).
    glGenBuffers(1, &model->faces_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->faces_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * face_count * sizeof *faces, faces, GL_STATIC_DRAW);

    // Book-keeping
    model->vertex_count = vertex_count;
    model->face_count = face_count;

    return model;
}

void model_destroy(struct model *model) {
    glDeleteBuffers(1, &model->vertices_buffer_id);
    glDeleteBuffers(1, &model->faces_buffer_id);
    glDeleteBuffers(1, &model->texture_uvs_buffer_id);
    glDeleteBuffers(1, &model->normals_buffer_id);
    glDeleteVertexArrays(1, &model->vertex_array_id);
    free(model);
}

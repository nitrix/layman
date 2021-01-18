#include "layman.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <stdlib.h>

struct layman_mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint ebo_indices;
	size_t indices_count;
};

struct layman_mesh *layman_mesh_create_from_raw(const float *vertices, size_t vertices_count, size_t vertices_stride, const float *normals, size_t normals_count, size_t normals_stride, const unsigned short *indices, size_t indices_count) {
	struct layman_mesh *mesh = malloc(sizeof *mesh);
	if (!mesh) {
		return NULL;
	}

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_POSITION);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, vertices_stride, 0);

	glGenBuffers(1, &mesh->vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals_count * 3 * sizeof(float), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, false, normals_stride, 0);

	glGenBuffers(1, &mesh->ebo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * 3 * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	mesh->indices_count = indices_count;

	return mesh;
}

void layman_mesh_destroy(struct layman_mesh *mesh) {
	glDeleteBuffers(1, &mesh->ebo_indices);
	glDeleteBuffers(1, &mesh->vbo_positions);
	glDeleteBuffers(1, &mesh->vbo_normals);
	glDeleteVertexArrays(1, &mesh->vao);
}

void layman_mesh_render(const struct layman_mesh *mesh) {
	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}
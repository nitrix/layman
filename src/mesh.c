#include "layman.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <stdlib.h>

struct layman_mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint ebo_indices;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	size_t indices_count;
};

struct layman_mesh *layman_mesh_create_from_raw(const float *vertices, size_t vertices_count, size_t vertices_stride, const float *normals, size_t normals_count, size_t normals_stride, const float *uvs, size_t uvs_count, size_t uvs_stride, const unsigned short *indices, size_t indices_count, const float *tangents, size_t tangents_count, size_t tangents_stride) {
	struct layman_mesh *mesh = malloc(sizeof *mesh);
	if (!mesh) {
		return NULL;
	}

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, vertices_stride, 0);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_POSITION);

	glGenBuffers(1, &mesh->vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals_count * 3 * sizeof(float), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, false, normals_stride, 0);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_NORMAL);

	glGenBuffers(1, &mesh->vbo_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, uvs_count * 2 * sizeof(float), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_UV, 2, GL_FLOAT, false, uvs_stride, 0);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_UV);

	glGenBuffers(1, &mesh->ebo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * 3 * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	mesh->indices_count = indices_count;

	glGenBuffers(1, &mesh->vbo_tangents);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, tangents_count * 3 * sizeof(float), tangents, GL_STATIC_DRAW);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_TANGENT, 3, GL_FLOAT, false, tangents_stride, 0);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_TANGENT);

	// Bitangents.
	// bitangent = cross(normal, tangent.xyz) * tangent.w
	float *bitangents = malloc(tangents_count * 3 * sizeof *bitangents);
	if (!bitangents) {
		// TODO: Handle failure.
	}
	size_t bitangents_count = tangents_count;
	size_t bitangents_stride = tangents_stride;

	for (size_t i = 0; i < tangents_count; i++) {
		// FIXME: Dirty conversion?
		const struct layman_vector_3f *a = normals + i * 3;
		const struct layman_vector_3f *b = tangents + i * 3;
		struct layman_vector_3f *r = bitangents + i * 3;
		layman_vector_3f_cross_3f(a, b, r);
	}

	glGenBuffers(1, &mesh->vbo_bitangents);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_bitangents);
	glBufferData(GL_ARRAY_BUFFER, bitangents_count * 3 * sizeof(float), bitangents, GL_STATIC_DRAW);
	glVertexAttribPointer(LAYMAN_MESH_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, false, bitangents_stride, 0);
	glEnableVertexAttribArray(LAYMAN_MESH_ATTRIBUTE_BITANGENT);

	return mesh;
}

void layman_mesh_destroy(struct layman_mesh *mesh) {
	glDeleteBuffers(1, &mesh->ebo_indices);
	glDeleteBuffers(1, &mesh->vbo_positions);
	glDeleteBuffers(1, &mesh->vbo_normals);
	glDeleteBuffers(1, &mesh->vbo_tangents);
	glDeleteBuffers(1, &mesh->vbo_bitangents);
	glDeleteVertexArrays(1, &mesh->vao);
}

void layman_mesh_render(const struct layman_mesh *mesh) {
	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}
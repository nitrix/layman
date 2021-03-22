#ifndef MESH_H
#define MESH_H

#include "material.h"
#include <glad/glad.h>
#include <stdlib.h>

enum mesh_attribute {
	MESH_ATTRIBUTE_POSITION,
	MESH_ATTRIBUTE_UV,
	MESH_ATTRIBUTE_NORMAL,
	MESH_ATTRIBUTE_TANGENT,
};

struct mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint ebo_indices;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	size_t indices_count;

	struct shader *shader;
	const struct material *material;
};

void mesh_switch(const struct mesh *mesh);

struct mesh *mesh_create(void);

struct mesh *mesh_create_from_raw(
	// Vertices.
	const float *vertices, size_t vertices_count, size_t vertices_stride,
	// Normals.
	const float *normals, size_t normals_count, size_t normals_stride,
	// UVs.
	const float *uvs, size_t uvs_count, size_t uvs_stride,
	// Indices.
	const unsigned short *indices, size_t indices_count,
	// Tangents.
	const float *tangents, size_t tangents_count, size_t tangents_stride);

void mesh_destroy(struct mesh *mesh);

#endif

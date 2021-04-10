#ifndef MESH_H
#define MESH_H

#include "glad/glad.h"
#include "material.h"
#include <stdbool.h>

enum mesh_attribute {
	MESH_ATTRIBUTE_POSITION,
	MESH_ATTRIBUTE_UV,
	MESH_ATTRIBUTE_NORMAL,
	MESH_ATTRIBUTE_TANGENT,
	MESH_ATTRIBUTE_WEIGHTS,
	MESH_ATTRIBUTE_JOINTS,
	MESH_ATTRIBUTE_COLORS,
};

struct mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint ebo_indices;
	GLuint vbo_tangents;
	GLuint vbo_weights;
	GLuint vbo_joints;
	GLuint vbo_colors;

	size_t indices_count;
	GLenum indices_type;

	struct shader *shader;
	struct material material;

	mat4 initial_transform;
};

bool mesh_init(struct mesh *mesh);
void mesh_fini(struct mesh *mesh);
void mesh_provide_vertices(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_normals(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_uvs(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_indices(struct mesh *mesh, const void *data, size_t count, GLenum type);
void mesh_provide_tangents(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_weights(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_joints(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_colors(struct mesh *mesh, const float *data, size_t count, size_t stride, int components);
void mesh_switch(const struct mesh *mesh);

#endif

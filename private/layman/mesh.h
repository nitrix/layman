#ifndef LAYMAN_PRIVATE_MESH_H
#define LAYMAN_PRIVATE_MESH_H

#include <glad/glad.h>

struct layman_mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint ebo_indices;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	size_t indices_count;

	struct layman_shader *shader;
	const struct layman_material *material;
};

void layman_mesh_switch(const struct layman_mesh *mesh);

#endif

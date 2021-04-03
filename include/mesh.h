#ifndef MESH_H
#define MESH_H

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
	size_t indices_count;

	struct shader *shader;
	struct material material;
};

bool mesh_init(struct mesh *mesh);
void mesh_fini(struct mesh *mesh);
void mesh_provide_vertices(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_normals(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_uvs(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_provide_indices(struct mesh *mesh, const unsigned short *data, size_t count);
void mesh_provide_tangents(struct mesh *mesh, const float *data, size_t count, size_t stride);
void mesh_switch(const struct mesh *mesh);

#endif

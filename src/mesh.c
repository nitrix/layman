#include "client.h"

INCBIN(shaders_pbr_main_vert, "../shaders/pbr/main.vert");
INCBIN(shaders_pbr_main_frag, "../shaders/pbr/main.frag");

bool mesh_init(struct mesh *mesh) {
	mesh->vao = 0;
	mesh->vbo_positions = 0;
	mesh->vbo_normals = 0;
	mesh->vbo_uvs = 0;
	mesh->ebo_indices = 0;
	mesh->vbo_tangents = 0;

	// FIXME: Currently, each mesh has its own material, but I believe glTF is able to share common materials.
	// If so, there should probably be a material manager of some sort.
	material_init(&mesh->material);

	// TODO: Shader caching based on the #defines that it needs. Each mesh having their own shader is a bit wasteful otherwise.
	mesh->shader = shader_load_from_memory(shaders_pbr_main_vert_data, shaders_pbr_main_vert_size, shaders_pbr_main_frag_data, shaders_pbr_main_frag_size, NULL, 0);
	if (!mesh->shader) {
		return false;
	}

	// Vertex Array Object (VAO).
	// This contains multiple buffers and is the preferred way to change from one group of buffers to another when rendering models.
	glGenVertexArrays(1, &mesh->vao);

	return true;
}

void mesh_provide_vertices(struct mesh *mesh, const float *data, size_t count, size_t stride) {
	mesh_switch(mesh);

	glGenBuffers(1, &mesh->vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof (float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, stride, 0);
	glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION);
}

void mesh_provide_normals(struct mesh *mesh, const float *data, size_t count, size_t stride) {
	mesh_switch(mesh);

	glGenBuffers(1, &mesh->vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof (float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, false, stride, 0);
	glEnableVertexAttribArray(MESH_ATTRIBUTE_NORMAL);
}

void mesh_provide_uvs(struct mesh *mesh, const float *data, size_t count, size_t stride) {
	mesh_switch(mesh);

	glGenBuffers(1, &mesh->vbo_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof (float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(MESH_ATTRIBUTE_UV, 2, GL_FLOAT, false, stride, 0);
	glEnableVertexAttribArray(MESH_ATTRIBUTE_UV);
}

void mesh_provide_indices(struct mesh *mesh, const unsigned short *data, size_t count) {
	mesh_switch(mesh);

	// Most buffers get assigned to a shader attribute (aka shader input variables) using glVertexAttribPointer.
	// The one exception is this indice buffer. Instead, we pass the count explicitly during the render call (glDrawElements).
	glGenBuffers(1, &mesh->ebo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * 1 * sizeof (unsigned short), data, GL_STATIC_DRAW);
	mesh->indices_count = count;
}

void mesh_provide_tangents(struct mesh *mesh, const float *data, size_t count, size_t stride) {
	mesh_switch(mesh);

	glGenBuffers(1, &mesh->vbo_tangents);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof (float), data, GL_STATIC_DRAW);
	glVertexAttribPointer(MESH_ATTRIBUTE_TANGENT, 4, GL_FLOAT, false, stride, 0);
	glEnableVertexAttribArray(MESH_ATTRIBUTE_TANGENT);
}

void mesh_switch(const struct mesh *mesh) {
	glBindVertexArray(mesh->vao);
	material_switch(&mesh->material);
}

void mesh_fini(struct mesh *mesh) {
	material_fini(&mesh->material);

	shader_destroy(mesh->shader);

	glDeleteBuffers(1, &mesh->vbo_positions);
	glDeleteBuffers(1, &mesh->ebo_indices);

	if (mesh->vbo_normals) {
		glDeleteBuffers(1, &mesh->vbo_normals);
	}

	if (mesh->vbo_normals) {
		glDeleteBuffers(1, &mesh->vbo_uvs);
	}

	if (mesh->vbo_tangents) {
		glDeleteBuffers(1, &mesh->vbo_tangents);
	}

	glDeleteVertexArrays(1, &mesh->vao);
}

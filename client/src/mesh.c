#include "incbin.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include <stdlib.h>

INCBIN(shaders_pbr_main_vert, "../shaders/pbr/main.vert");
INCBIN(shaders_pbr_main_frag, "../shaders/pbr/main.frag");

/*
   float *generate_bitangents(const float *normals, const float *tangents, size_t tangents_count) {
        float *bitangents = malloc(tangents_count * 3 * sizeof *bitangents);
        if (!bitangents) {
                return NULL;
        }

        // Bitangents are generated from normals and tangents.
        // bitangent = cross(normal, tangent.xyz) * tangent.w

        for (size_t i = 0; i < tangents_count; i++) {
                // FIXME: Dirty conversion?
                // FIXME: Formatting.
                struct vector_3f *normal = (struct vector_3f *) (normals + i * 3);
                struct vector_3f *tangent3f = (struct vector_3f *) (tangents + i * 4);
                struct vector_4f *tangent4f = (struct vector_4f *) (tangents + i * 4);
                struct vector_3f *bitangent = (struct vector_3f *) (bitangents + i * 3);
                vector_3f_cross_3f(normal, tangent3f, bitangent);
                vector_3f_multiply_1f(bitangent, tangent4f->d[3], bitangent);
        }

        return bitangents;
   }
 */

struct mesh *mesh_create(void) {
	struct mesh *mesh = malloc(sizeof *mesh);
	if (!mesh) {
		return NULL;
	}

	mesh->vao = 0;
	mesh->vbo_positions = 0;
	mesh->vbo_normals = 0;
	mesh->vbo_uvs = 0;
	mesh->ebo_indices = 0;
	mesh->vbo_tangents = 0;

	mesh->material = NULL;

	// TODO: Shader caching based on the #defines that it needs. Each mesh having their own shader is a bit wasteful otherwise.
	mesh->shader = shader_load_from_memory(shaders_pbr_main_vert_data, shaders_pbr_main_vert_size, shaders_pbr_main_frag_data, shaders_pbr_main_frag_size, NULL, 0);
	if (!mesh->shader) {
		free(mesh);
		return NULL;
	}

	// Vertex Array Object (VAO).
	// This contains multiple buffers and is the preferred way to change from one group of buffers to another when rendering models.
	glGenVertexArrays(1, &mesh->vao);

	return mesh;
}

struct mesh *mesh_create_from_raw(const float *vertices, size_t vertices_count, size_t vertices_stride, const float *normals, size_t normals_count, size_t normals_stride, const float *uvs, size_t uvs_count, size_t uvs_stride, const unsigned short *indices, size_t indices_count, const float *tangents, size_t tangents_count, size_t tangents_stride) {
	struct mesh *mesh = mesh_create();
	if (!mesh) {
		return NULL;
	}

	mesh_switch(mesh);

	// Vertices.
	glGenBuffers(1, &mesh->vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * 3 * sizeof (float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(MESH_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, vertices_stride, 0);
	glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION);

	// Normals.
	if (normals_count > 0) {
		glGenBuffers(1, &mesh->vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals_count * 3 * sizeof (float), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(MESH_ATTRIBUTE_NORMAL, 3, GL_FLOAT, false, normals_stride, 0);
		glEnableVertexAttribArray(MESH_ATTRIBUTE_NORMAL);
	}

	// UVs.
	if (uvs_count > 0) {
		glGenBuffers(1, &mesh->vbo_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, uvs_count * 2 * sizeof (float), uvs, GL_STATIC_DRAW);
		glVertexAttribPointer(MESH_ATTRIBUTE_UV, 2, GL_FLOAT, false, uvs_stride, 0);
		glEnableVertexAttribArray(MESH_ATTRIBUTE_UV);
	}

	// Indices.
	// Most buffers get assigned to a shader attribute (aka shader input variables). The one exception is the indice buffer.
	// That guy gets used by glDrawElements for the rendering, but isn't useful to the shader, as far as I know.
	glGenBuffers(1, &mesh->ebo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * 1 * sizeof (unsigned short), indices, GL_STATIC_DRAW);
	mesh->indices_count = indices_count;

	// Tangents.
	if (tangents_count > 0) {
		glGenBuffers(1, &mesh->vbo_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents_count * 4 * sizeof (float), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(MESH_ATTRIBUTE_TANGENT, 4, GL_FLOAT, false, tangents_stride, 0);
		glEnableVertexAttribArray(MESH_ATTRIBUTE_TANGENT);
	}

	return mesh;
}

void mesh_switch(const struct mesh *new) {
	thread_local static const struct mesh *current;

	if (current == new) {
		return;
	}

	current = new;

	if (new) {
		glBindVertexArray(new->vao);
		material_switch(new->material);
	} else {
		shader_switch(NULL);
		material_switch(NULL);
	}
}

void mesh_destroy(struct mesh *mesh) {
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

#ifndef LAYMAN_MESH_H
#define LAYMAN_MESH_H

#include "material.h"
#include <stdlib.h>

enum layman_mesh_attribute {
	LAYMAN_MESH_ATTRIBUTE_POSITION,
	LAYMAN_MESH_ATTRIBUTE_UV,
	LAYMAN_MESH_ATTRIBUTE_NORMAL,
	LAYMAN_MESH_ATTRIBUTE_TANGENT,
	// LAYMAN_MESH_ATTRIBUTE_BITANGENT,
};

// TODO: Documentation.
struct layman_mesh *layman_mesh_create(void);

// TODO: Documentation.
struct layman_mesh *layman_mesh_create_from_raw(
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

// TODO: Documentation.
void layman_mesh_destroy(struct layman_mesh *mesh);

void layman_mesh_assign_material(struct layman_mesh *mesh, const struct layman_material *material);

#endif

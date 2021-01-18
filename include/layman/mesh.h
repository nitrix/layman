#ifndef LAYMAN_MESH_H
#define LAYMAN_MESH_H

#include "material.h"
#include <stdlib.h>

enum layman_mesh_attribute {
	LAYMAN_MESH_ATTRIBUTE_POSITION,
	LAYMAN_MESH_ATTRIBUTE_UV,
	LAYMAN_MESH_ATTRIBUTE_NORMAL,
	LAYMAN_MESH_ATTRIBUTE_TANGENT,
};

struct layman_mesh *layman_mesh_create_from_raw(const float *vertices, size_t vertices_count, size_t vertices_stride, const float *normals, size_t normals_count, size_t normals_stride, const unsigned short *indices, size_t indices_count);
void layman_mesh_destroy(struct layman_mesh *mesh);

void layman_mesh_use(const struct layman_mesh *mesh);
void layman_mesh_unuse(const struct layman_mesh *mesh);

size_t layman_mesh_triangle_count(const struct layman_mesh *mesh);
size_t layman_mesh_indices_count(const struct layman_mesh *mesh);

#endif
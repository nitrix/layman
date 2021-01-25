#ifndef LAYMAN_MATRIX_H
#define LAYMAN_MATRIX_H

#include "vector.h"

struct layman_matrix_4f {
	float d[16];
};

struct layman_matrix_4f layman_matrix_4f_identity(void);
struct layman_matrix_4f layman_matrix_4f_create_from_transformation(struct layman_vector_3f translation, struct layman_vector_3f rotation, float scale);
void layman_matrix_4f_translate(struct layman_matrix_4f *m, struct layman_vector_3f v);
void layman_matrix_4f_dot_product(struct layman_matrix_4f *p, const struct layman_matrix_4f *o);
void layman_matrix_4f_scale(struct layman_matrix_4f *m, float sx, float sy, float sz);
void layman_matrix_4f_rotate_x(struct layman_matrix_4f *m, float r);
void layman_matrix_4f_rotate_y(struct layman_matrix_4f *m, float r);
void layman_matrix_4f_rotate_z(struct layman_matrix_4f *m, float r);

#endif
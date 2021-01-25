#include "layman.h"
#include <math.h>

struct layman_matrix_4f layman_matrix_4f_identity(void) {
	struct layman_matrix_4f m = {
	    .d = {
	        [0] = 1.0f,
	        [5] = 1.0f,
	        [10] = 1.0f,
	        [15] = 1.0f,
	    }};

	return m;
}

struct layman_matrix_4f layman_matrix_4f_create_from_transformation(struct layman_vector_3f translation, struct layman_vector_3f rotation, float scale) {
	struct layman_matrix_4f m = layman_matrix_4f_identity();

	layman_matrix_4f_translate(&m, translation);
	layman_matrix_4f_rotate_x(&m, rotation.d[0]);
	layman_matrix_4f_rotate_y(&m, rotation.d[1]);
	layman_matrix_4f_rotate_z(&m, rotation.d[2]);
	layman_matrix_4f_scale(&m, scale, scale, scale);

	return m;
}

void layman_matrix_4f_rotate_x(struct layman_matrix_4f *m, float r) {
	struct layman_matrix_4f t = {
	    .d = {
	        [0] = 1.0f,
	        [5] = cosf(r),
	        [6] = -sinf(r),
	        [9] = sinf(r),
	        [10] = cosf(r),
	        [15] = 1.0f,
	    },
	};

	layman_matrix_4f_dot_product(m, &t);
}

void layman_matrix_4f_rotate_y(struct layman_matrix_4f *m, float r) {
	struct layman_matrix_4f t = {
	    .d = {
	        [0] = cosf(r),
	        [2] = sinf(r),
	        [5] = 1.0f,
	        [8] = -sinf(r),
	        [10] = cosf(r),
	        [15] = 1.0f,
	    },
	};

	layman_matrix_4f_dot_product(m, &t);
}

void layman_matrix_4f_rotate_z(struct layman_matrix_4f *m, float r) {
	struct layman_matrix_4f t = {
	    .d = {
	        [0] = cosf(r),
	        [1] = -sinf(r),
	        [4] = sinf(r),
	        [5] = cosf(r),
	        [10] = 1.0f,
	        [15] = 1.0f,
	    },
	};

	layman_matrix_4f_dot_product(m, &t);
}

void layman_matrix_4f_translate(struct layman_matrix_4f *m, struct layman_vector_3f v) {
	struct layman_matrix_4f t = layman_matrix_4f_identity();

	t.d[12] = v.d[0];
	t.d[13] = v.d[1];
	t.d[14] = v.d[2];

	layman_matrix_4f_dot_product(m, &t);
}

void layman_matrix_4f_scale(struct layman_matrix_4f *m, float sx, float sy, float sz) {
	struct layman_matrix_4f t = layman_matrix_4f_identity();

	t.d[0] = sx;
	t.d[5] = sy;
	t.d[10] = sz;
	t.d[15] = 1.0f;

	layman_matrix_4f_dot_product(m, &t);
}

void layman_matrix_4f_dot_product(struct layman_matrix_4f *p, const struct layman_matrix_4f *o) {
	struct layman_matrix_4f m = *p;

	struct layman_matrix_4f t = {
	    .d = {
	        [0] = (m.d[0] * o->d[0]) + (m.d[1] * o->d[4]) + (m.d[2] * o->d[8]) + (m.d[3] * o->d[12]),
	        [1] = (m.d[0] * o->d[1]) + (m.d[1] * o->d[5]) + (m.d[2] * o->d[9]) + (m.d[3] * o->d[13]),
	        [2] = (m.d[0] * o->d[2]) + (m.d[1] * o->d[6]) + (m.d[2] * o->d[10]) + (m.d[3] * o->d[14]),
	        [3] = (m.d[0] * o->d[3]) + (m.d[1] * o->d[7]) + (m.d[2] * o->d[11]) + (m.d[3] * o->d[15]),

	        [4] = (m.d[4] * o->d[0]) + (m.d[5] * o->d[4]) + (m.d[6] * o->d[8]) + (m.d[7] * o->d[12]),
	        [5] = (m.d[4] * o->d[1]) + (m.d[5] * o->d[5]) + (m.d[6] * o->d[9]) + (m.d[7] * o->d[13]),
	        [6] = (m.d[4] * o->d[2]) + (m.d[5] * o->d[6]) + (m.d[6] * o->d[10]) + (m.d[7] * o->d[14]),
	        [7] = (m.d[4] * o->d[3]) + (m.d[5] * o->d[7]) + (m.d[6] * o->d[11]) + (m.d[7] * o->d[15]),

	        [8] = (m.d[8] * o->d[0]) + (m.d[9] * o->d[4]) + (m.d[10] * o->d[8]) + (m.d[11] * o->d[12]),
	        [9] = (m.d[8] * o->d[1]) + (m.d[9] * o->d[5]) + (m.d[10] * o->d[9]) + (m.d[11] * o->d[13]),
	        [10] = (m.d[8] * o->d[2]) + (m.d[9] * o->d[6]) + (m.d[10] * o->d[10]) + (m.d[11] * o->d[14]),
	        [11] = (m.d[8] * o->d[3]) + (m.d[9] * o->d[7]) + (m.d[10] * o->d[11]) + (m.d[11] * o->d[15]),

	        [12] = (m.d[12] * o->d[0]) + (m.d[13] * o->d[4]) + (m.d[14] * o->d[8]) + (m.d[15] * o->d[12]),
	        [13] = (m.d[12] * o->d[1]) + (m.d[13] * o->d[5]) + (m.d[14] * o->d[9]) + (m.d[15] * o->d[13]),
	        [14] = (m.d[12] * o->d[2]) + (m.d[13] * o->d[6]) + (m.d[14] * o->d[10]) + (m.d[15] * o->d[14]),
	        [15] = (m.d[12] * o->d[3]) + (m.d[13] * o->d[7]) + (m.d[14] * o->d[11]) + (m.d[15] * o->d[15]),
	    },
	};

	*p = t;
}
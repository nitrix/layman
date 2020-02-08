#ifndef LEARNGL_MATRIX_H
#define LEARNGL_MATRIX_H

#include "vector.h"

struct matrix4f {
    float x1, x2, x3, x4;
    float y1, y2, y3, y4;
    float z1, z2, z3, z4;
    float w1, w2, w3, w4;
};

struct matrix4f matrix_identity(void);
void matrix_translate(struct matrix4f *m, const struct vector3f *v);
void matrix_multiply(struct matrix4f *p, const struct matrix4f *o);
void matrix_scale(struct matrix4f *m, float sx, float sy, float sz);
void matrix_rotate_x(struct matrix4f *m, float r);
void matrix_rotate_y(struct matrix4f *m, float r);
void matrix_rotate_z(struct matrix4f *m, float r);

#endif

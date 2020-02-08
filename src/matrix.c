#include "matrix.h"
#include "math.h"

struct matrix4f matrix_identity(void) {
    struct matrix4f m = {
        .x1 = 1.0f,
        .y2 = 1.0f,
        .z3 = 1.0f,
        .w4 = 1.0f,
    };

    return m;
}

void matrix_rotate_x(struct matrix4f *m, float r) {
    struct matrix4f t = {
        .x1 = 1,
        .x2 = 0,
        .x3 = 0,
        .y1 = 0,
        .y2 = cosf(r),
        .y3 = -sinf(r),
        .z1 = 0,
        .z2 = sinf(r),
        .z3 = cosf(r),
        .w1 = 0,
        .w2 = 0,
        .w3 = 0,
        .w4 = 1,
    };

    matrix_dot_product(m, &t);
}

void matrix_rotate_y(struct matrix4f *m, float r) {
    struct matrix4f t = {
        .x1 = cosf(r),
        .x2 = 0,
        .x3 = sinf(r),
        .y1 = 0,
        .y2 = 1,
        .y3 = 0,
        .z1 = -sinf(r),
        .z2 = 0,
        .z3 = cosf(r),
        .w1 = 0,
        .w2 = 0,
        .w3 = 0,
        .w4 = 1,
    };

    matrix_dot_product(m, &t);
}

void matrix_rotate_z(struct matrix4f *m, float r) {
    struct matrix4f t = {
        .x1 = cosf(r),
        .x2 = -sinf(r),
        .x3 = 0,
        .y1 = sinf(r),
        .y2 = cosf(r),
        .y3 = 0,
        .z1 = 0,
        .z2 = 0,
        .z3 = 1,
        .w1 = 0,
        .w2 = 0,
        .w3 = 0,
        .w4 = 1,
    };

    matrix_dot_product(m, &t);
}

void matrix_translate(struct matrix4f *m, const struct vector3f *v) {
    struct matrix4f t = matrix_identity();

    t.w1 = v->x;
    t.w2 = v->y;
    t.w3 = v->z;

    matrix_dot_product(m, &t);
}

void matrix_scale(struct matrix4f *m, float sx, float sy, float sz) {
    struct matrix4f t = matrix_identity();
    t.x1 = sx;
    t.y2 = sy;
    t.z3 = sz;
    t.w4 = 1;

    matrix_dot_product(m, &t);
}

// TODO: Misnamed?
void matrix_dot_product(struct matrix4f *p, const struct matrix4f *o) {
    struct matrix4f m = *p;

    struct matrix4f t = {
        .x1 = (m.x1 * o->x1) + (m.x2 * o->y1) + (m.x3 * o->z1) + (m.x4 * o->w1),
        .x2 = (m.x1 * o->x2) + (m.x2 * o->y2) + (m.x3 * o->z2) + (m.x4 * o->w2),
        .x3 = (m.x1 * o->x3) + (m.x2 * o->y3) + (m.x3 * o->z3) + (m.x4 * o->w3),
        .x4 = (m.x1 * o->x4) + (m.x2 * o->y4) + (m.x3 * o->z4) + (m.x4 * o->w4),

        .y1 = (m.y1 * o->x1) + (m.y2 * o->y1) + (m.y3 * o->z1) + (m.y4 * o->w1),
        .y2 = (m.y1 * o->x2) + (m.y2 * o->y2) + (m.y3 * o->z2) + (m.y4 * o->w2),
        .y3 = (m.y1 * o->x3) + (m.y2 * o->y3) + (m.y3 * o->z3) + (m.y4 * o->w3),
        .y4 = (m.y1 * o->x4) + (m.y2 * o->y4) + (m.y3 * o->z4) + (m.y4 * o->w4),

        .z1 = (m.z1 * o->x1) + (m.z2 * o->y1) + (m.z3 * o->z1) + (m.z4 * o->w1),
        .z2 = (m.z1 * o->x2) + (m.z2 * o->y2) + (m.z3 * o->z2) + (m.z4 * o->w2),
        .z3 = (m.z1 * o->x3) + (m.z2 * o->y3) + (m.z3 * o->z3) + (m.z4 * o->w3),
        .z4 = (m.z1 * o->x4) + (m.z2 * o->y4) + (m.z3 * o->z4) + (m.z4 * o->w4),

        .w1 = (m.w1 * o->x1) + (m.w2 * o->y1) + (m.w3 * o->z1) + (m.w4 * o->w1),
        .w2 = (m.w1 * o->x2) + (m.w2 * o->y2) + (m.w3 * o->z2) + (m.w4 * o->w2),
        .w3 = (m.w1 * o->x3) + (m.w2 * o->y3) + (m.w3 * o->z3) + (m.w4 * o->w3),
        .w4 = (m.w1 * o->x4) + (m.w2 * o->y4) + (m.w3 * o->z4) + (m.w4 * o->w4),
    };

    *p = t;
}
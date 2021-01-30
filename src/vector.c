#include "layman.h"

void layman_vector_3f_add_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r) {
	r->d[0] = a->d[0] + b->d[0];
	r->d[1] = a->d[1] + b->d[1];
	r->d[2] = a->d[2] + b->d[2];
}

void layman_vector_3f_sub_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r) {
	r->d[0] = a->d[0] - b->d[0];
	r->d[1] = a->d[1] - b->d[1];
	r->d[2] = a->d[2] - b->d[2];
}

void layman_vector_3f_multiply_1f(const struct layman_vector_3f *a, float b, struct layman_vector_3f *r) {
	r->d[0] = a->d[0] * b;
	r->d[1] = a->d[1] * b;
	r->d[2] = a->d[2] * b;
}

void layman_vector_3f_cross_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r) {
	r->d[0] = a->d[1] * b->d[2] - a->d[2] * b->d[1];
	r->d[1] = a->d[2] * b->d[0] - a->d[0] * b->d[2];
	r->d[2] = a->d[0] * b->d[1] - a->d[1] * b->d[0];
}

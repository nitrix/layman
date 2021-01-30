#ifndef LAYMAN_VECTOR_H
#define LAYMAN_VECTOR_H

struct layman_vector_3f {
	float d[3];
};

struct layman_vector_4f {
	float d[4];
};

// FIXME: Formatting.
#define LAYMAN_VECTOR_3F(x, y, z) (struct layman_vector_3f) { .d = {x, y, z}}
#define LAYMAN_VECTOR_4F(x, y, z, w) (struct layman_vector_4f) { .d = {x, y, z, w}}

void layman_vector_3f_add_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);
void layman_vector_3f_sub_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);
void layman_vector_3f_cross_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);
void layman_vector_3f_multiply_1f(const struct layman_vector_3f *a, float b, struct layman_vector_3f *r);

#endif

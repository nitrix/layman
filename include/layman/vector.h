#ifndef LAYMAN_VECTOR_H
#define LAYMAN_VECTOR_H

struct layman_vector_3f {
	float d[3];
};

#define LAYMAN_VECTOR_3F(x, y, z) (struct layman_vector_3f) { .d = {x, y, z}}

void layman_vector_3f_add_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);
void layman_vector_3f_sub_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);
void layman_vector_3f_cross_3f(const struct layman_vector_3f *a, const struct layman_vector_3f *b, struct layman_vector_3f *r);

#endif

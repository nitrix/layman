#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "texture.h"

struct environment {
	struct texture cubemap;

	size_t mip_count;
	struct texture lambertian;
	struct texture ggx;
	struct texture ggx_lut;
	struct texture charlie;
	struct texture charlie_lut;
};

bool environment_init_from_file(struct environment *environment, const char *filepath);
void environment_fini(struct environment *environment);

void environment_debug(const struct environment *environment);
void environment_switch(const struct environment *environment);

#endif

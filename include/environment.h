#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

struct environment {
	struct texture *cubemap;

	size_t mip_count;
	struct texture *lambertian;
	struct texture *lambertian_lut; // TODO: Remove, that's not a thing.
	struct texture *ggx;
	struct texture *ggx_lut;
	struct texture *charlie;
	struct texture *charlie_lut;
};

void environment_debug(const struct environment *environment);

void environment_switch(const struct environment *new);

struct environment *environment_create_from_hdr(const char *filepath);
void environment_destroy(struct environment *environment);

void renderCube(); // FIXME: MOVE ME SOMEWHERE!

#endif

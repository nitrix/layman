#ifndef LAYMAN_PRIVATE_ENVIRONMENT_H
#define LAYMAN_PRIVATE_ENVIRONMENT_H

struct layman_environment {
	struct layman_texture *cubemap;

	size_t mip_count;
	struct layman_texture *lambertian;
	struct layman_texture *lambertian_lut; // TODO: Remove, that's not a thing.
	struct layman_texture *ggx;
	struct layman_texture *ggx_lut;
	struct layman_texture *charlie;
	struct layman_texture *charlie_lut;
};

void layman_environment_debug(const struct layman_environment *environment); // TODO: remove?

void layman_environment_switch(const struct layman_environment *new);

#endif

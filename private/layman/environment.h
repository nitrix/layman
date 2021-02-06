#ifndef LAYMAN_PRIVATE_ENVIRONMENT_H
#define LAYMAN_PRIVATE_ENVIRONMENT_H

struct layman_environment {
	struct layman_texture *lambertian;
	struct layman_texture *lambertian_lut;
	struct layman_texture *ggx;
	struct layman_texture *ggx_lut;
	struct layman_texture *charlie;
	struct layman_texture *charlie_lut;
};

void layman_environment_debug(const struct layman_environment *environment);

#endif

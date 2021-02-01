#include "layman.h"
#include "stb_image.h"
#include <stdlib.h>

struct layman_environment {
	struct layman_texture *texture;
};

struct layman_environment *layman_environment_create_from_hdr(const char *filepath) {
	struct layman_environment *environment = malloc(sizeof *environment);
	if (!environment) {
		return NULL;
	}

	environment->texture = NULL;

	stbi_set_flip_vertically_on_load(true);
	int w, h, c;
	float *data = stbi_loadf(filepath, &w, &h, &c, 0);

	if (!data) {
		free(environment);
		return NULL;
	}

	struct layman_texture *texture = layman_texture_create(LAYMAN_TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN);
	if (!texture) {
		free(environment);
		free(data);
		return NULL;
	}

	// FIXME: Styling.
	layman_texture_provide_data(
	        texture, data, w, h,
	        LAYMAN_TEXTURE_DATA_TYPE_FLOAT,
	        LAYMAN_TEXTURE_DATA_FORMAT_RGB,
	        LAYMAN_TEXTURE_DATA_INTERNAL_FORMAT_RGB16F);

	stbi_image_free(data);

	return environment;
}

void layman_environment_destroy(struct layman_environment *environment) {
	layman_texture_destroy(environment->texture);
	free(environment);
}

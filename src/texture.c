#include <stdlib.h>

struct layman_texture {};

struct layman_texture *layman_texture_create(void) {
	struct layman_texture *texture = malloc(sizeof *texture);
	if (!texture) {
		return NULL;
	}

	return texture;
}

void layman_texture_destroy(struct layman_texture *texture) {
	free(texture);
}
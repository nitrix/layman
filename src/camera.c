#include "layman.h"

struct layman_camera *layman_camera_create(void) {
	struct layman_camera *camera = malloc(sizeof *camera);
	if (!camera) {
		return NULL;
	}

	camera->position = LAYMAN_VECTOR_3F(0, 0, 0);

	return camera;
}

void layman_camera_destroy(struct layman_camera *camera) {
	if (!camera) {
		return;
	}

	free(camera);
}

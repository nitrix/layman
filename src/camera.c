#include "layman.h"

struct layman_camera *layman_camera_create(void) {
	struct layman_camera *camera = malloc(sizeof *camera);
	if (!camera) {
		return NULL;
	}

	glm_vec3_zero(camera->position);

	return camera;
}

void layman_camera_destroy(struct layman_camera *camera) {
	if (!camera) {
		return;
	}

	free(camera);
}

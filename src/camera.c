#include "layman.h"

struct layman_camera *layman_camera_create(void) {
	struct layman_camera *camera = malloc(sizeof *camera);
	if (!camera) {
		return NULL;
	}

	glm_vec3_zero(camera->translation);
	glm_vec3_zero(camera->rotation);

	return camera;
}

void layman_camera_destroy(struct layman_camera *camera) {
	if (!camera) {
		return;
	}

	free(camera);
}

void layman_camera_translation(struct layman_camera *camera, float x, float y, float z) {
	camera->translation[0] = x;
	camera->translation[1] = y;
	camera->translation[2] = z;
}

void layman_camera_rotation(struct layman_camera *camera, float x, float y, float z) {
	camera->rotation[0] = x;
	camera->rotation[1] = y;
	camera->rotation[2] = z;
}

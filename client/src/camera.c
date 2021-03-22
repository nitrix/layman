#include "camera.h"
#include "cglm/cglm.h"
#include <stdlib.h>

struct camera *camera_create(void) {
	struct camera *camera = malloc(sizeof *camera);
	if (!camera) {
		return NULL;
	}

	glm_vec3_zero(camera->translation);
	glm_vec3_zero(camera->rotation);

	return camera;
}

void camera_destroy(struct camera *camera) {
	if (!camera) {
		return;
	}

	free(camera);
}

void camera_translation(struct camera *camera, float x, float y, float z) {
	camera->translation[0] = x;
	camera->translation[1] = y;
	camera->translation[2] = z;
}

void camera_rotation(struct camera *camera, float x, float y, float z) {
	camera->rotation[0] = x;
	camera->rotation[1] = y;
	camera->rotation[2] = z;
}

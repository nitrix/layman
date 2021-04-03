#include "client.h"

void camera_init(struct camera *camera) {
	glm_vec3_zero(camera->translation);
	glm_vec3_zero(camera->rotation);
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

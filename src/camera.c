#include "client.h"

void camera_update_view_matrix(struct camera *camera) {
	glm_mat4_identity(camera->view_matrix);

	// TODO: Quaternions here too?
	glm_translate(camera->view_matrix, camera->translation);
	glm_rotate_z(camera->view_matrix, camera->rotation[2], camera->view_matrix);
	glm_rotate_y(camera->view_matrix, camera->rotation[1], camera->view_matrix);
	glm_rotate_x(camera->view_matrix, camera->rotation[0], camera->view_matrix);
	glm_mat4_inv(camera->view_matrix, camera->view_matrix);
}

void camera_init(struct camera *camera) {
	glm_vec3_zero(camera->translation);
	glm_vec3_zero(camera->rotation);
	camera_update_view_matrix(camera);
}

void camera_translation(struct camera *camera, float x, float y, float z) {
	camera->translation[0] = x;
	camera->translation[1] = y;
	camera->translation[2] = z;

	camera_update_view_matrix(camera);
}

void camera_rotation(struct camera *camera, float x, float y, float z) {
	camera->rotation[0] = x;
	camera->rotation[1] = y;
	camera->rotation[2] = z;

	camera_update_view_matrix(camera);
}

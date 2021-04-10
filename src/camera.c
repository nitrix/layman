#include "client.h"

void camera_update(struct camera *camera) {
	// Calculate the new eye position based on where it's situated relative to the center.

	float horizontal_distance = camera->eye_distance * cos(camera->eye_above);
	float vertical_distance = camera->eye_distance * sin(camera->eye_above);

	float x = horizontal_distance * sin(camera->eye_around + camera->center_rotation);
	float y = vertical_distance;
	float z = horizontal_distance * cos(camera->eye_around + camera->center_rotation);

	camera->eye[0] = x;
	camera->eye[1] = y;
	camera->eye[2] = z;
	camera->eye[0] += camera->center[0];
	camera->eye[1] += camera->center[1];
	camera->eye[2] += camera->center[2];

	// Re-calculate the view matrix.
	glm_lookat(camera->eye, camera->center, (vec3) { 0, 1, 0}, camera->view_matrix);
}

void camera_init(struct camera *camera) {
	glm_vec3_zero(camera->eye);
	glm_vec3_zero(camera->center);
	camera->eye_distance = 0.00001;
	camera_update(camera);
}

void camera_center_move_relative(struct camera *camera, float straight, float sideways) {
	// "Straight" (Forward/Backward).
	camera->center[0] += straight * sin(camera->center_rotation);
	camera->center[2] += straight * cos(camera->center_rotation);

	// "Sideways" (Left/Right).
	camera->center[0] += sideways * cos(camera->center_rotation);
	camera->center[2] += sideways * -sin(camera->center_rotation);

	// camera->translation[0] += x * sin(camera->center_rotation);
	camera_update(camera);
}

#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/cglm.h"

struct camera {
	// 3rd person camera.

	// The `center` is the reference point that the camera always looks at.
	// The reference point can turn, and that makes the camera turn too (always staying in its back).
	vec3 center;
	float center_rotation;

	// The `eye` is the actual camera, AUTOMATICALLY situated in the world.
	vec3 eye; // Computed by camera_update().
	float eye_around; // Additional rotation around the center.
	float eye_above; // Additional rotation above the center.
	float eye_distance; // Zoom lever from the center.

	mat4 view_matrix; // Computed by camera_update().
};

void camera_init(struct camera *camera);
void camera_center_move_relative(struct camera *camera, float straight, float sideways);
void camera_update(struct camera *camera);

#endif

#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/cglm.h"

struct camera {
	vec3 translation;
	vec3 rotation;
	mat4 view_matrix;
};

void camera_init(struct camera *camera);
void camera_translation(struct camera *, float x, float y, float z);
void camera_rotation(struct camera *, float x, float y, float z);
void camera_update_view_matrix(struct camera *camera);

#endif

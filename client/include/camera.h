#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/cglm.h"

struct camera {
	vec3 translation;
	vec3 rotation;
};

struct camera *camera_create(void);
void camera_destroy(struct camera *camera);

void camera_translation(struct camera *, float x, float y, float z);
void camera_rotation(struct camera *, float x, float y, float z);

#endif

#ifndef LAYMAN_PRIVATE_RENDERER_H
#define LAYMAN_PRIVATE_RENDERER_H

#include "glad/glad.h"

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

struct layman_renderer {
	// Viewport.
	int viewport_width;
	int viewport_height;

	// Projection matrix.
	float fov;
	float far_plane;
	float near_plane;

	// TODO: Temporary.
	double start_time;
	GLint viewProjectionMatrixLocation;
	GLint modelMatrixLocation;
	GLint normalMatrixLocation;
	GLint exposureLocation;

	float exposure;
};

void layman_renderer_switch(const struct layman_renderer *renderer);

#endif

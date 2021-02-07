#ifndef LAYMAN_PRIVATE_RENDERER_H
#define LAYMAN_PRIVATE_RENDERER_H

struct layman_renderer {
	// Viewport.
	float viewport_width;
	float viewport_height;

	// Projection matrix.
	float fov;
	float far_plane;
	float near_plane;

	// TODO: Does this belongs here?
	double start_time;
	float exposure;
};

void layman_renderer_switch(const struct layman_renderer *renderer);

#endif

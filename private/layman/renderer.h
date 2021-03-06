#ifndef LAYMAN_PRIVATE_RENDERER_H
#define LAYMAN_PRIVATE_RENDERER_H

#include "cimgui.h"
#include "cimgui_impl.h"

struct layman_renderer {
	// Viewport.
	float viewport_width;
	float viewport_height;

	// Projection matrix.
	float fov; // In degrees (gets converted to radians in a few places).
	float far_plane;
	float near_plane;

	// Debugging features.
	bool wireframe;

	// TODO: Does this belongs here?
	double start_time;
	float exposure;

	const struct layman_window *window;
	
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;
};

void layman_renderer_switch(const struct layman_renderer *renderer);

#endif

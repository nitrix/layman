#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "scene.h"
#include "window.h"

#include "cimgui.h"
#include "cimgui_impl.h"

#define FPS_HISTORY_MAX_COUNT 20

struct renderer {
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

	const struct window *window;

	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	float fps_history[FPS_HISTORY_MAX_COUNT];
	float fps_history_highest;
	size_t fps_history_total_count;
};

void renderer_switch(const struct renderer *renderer);

// TODO: Documentation.
struct renderer *renderer_create(const struct window *window);
void renderer_destroy(struct renderer *renderer);
void renderer_render(struct renderer *renderer, const struct camera *camera, const struct scene *scene);
void renderer_wireframe(struct renderer *renderer, bool enabled);

#endif

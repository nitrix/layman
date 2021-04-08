#ifndef RENDERER_H
#define RENDERER_H

#include "scene.h"
#include "ui.h"

#define FPS_HISTORY_MAX_COUNT 20

struct renderer {
	// Viewport.
	float viewport_width;
	float viewport_height;

	// Necessary for PBR.
	float exposure;

	// Projection matrix.
	float fov; // In degrees (gets converted to radians in a few places).
	float far_plane;
	float near_plane;
	mat4 projection_matrix;

	// Debugging features.
	bool wireframe;

	// Mouse picking.
	struct shader *plain_shader;
	uint32_t mousepicking_entity_id;
};

void renderer_init(struct renderer *renderer);
void renderer_fini(struct renderer *renderer);
void renderer_render(struct renderer *renderer, const struct camera *camera, const struct scene *scene);
void renderer_wireframe(struct renderer *renderer, bool enabled);
void renderer_switch(const struct renderer *renderer);
void renderer_update_projection_matrix(struct renderer *renderer);

#endif

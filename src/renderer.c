#include "layman.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

struct layman_renderer {
	struct layman_shader *pbr_shader;
};

struct layman_renderer *layman_renderer_create(void) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	free(renderer);
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_scene *scene) {
	(void) renderer; // Unused.

	// Clear the screen.
	glClearColor(0, 0, 0, 1); // Black.
	glClear(GL_COLOR_BUFFER_BIT);

	// Render entities.
	layman_scene_render_entities(scene);
}
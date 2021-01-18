#include "layman.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

struct layman_renderer {
	struct layman_shader *pbr_shader;
};

struct layman_renderer *layman_renderer_create(const struct layman_window *window) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	layman_window_use(window);

	renderer->pbr_shader = layman_shader_load_from_file("shaders/pbr.vert", "shaders/pbr.frag");
	if (!renderer->pbr_shader) {
		fprintf(stderr, "Unable to create the shader\n");
		layman_window_unuse(window);
		free(renderer);
		return NULL;
	}

	glClearColor(0, 0, 0, 1); // Black.

	layman_window_unuse(window);

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	free(renderer);
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_scene *scene) {
	(void) renderer; // Unused.
	glClear(GL_COLOR_BUFFER_BIT);
	layman_shader_use(renderer->pbr_shader);
	layman_scene_render(scene);
	layman_shader_unuse(renderer->pbr_shader);
}
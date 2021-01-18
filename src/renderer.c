#include "layman.h"
#include <glad/glad.h>
#include <stdlib.h>

struct layman_renderer {};

struct layman_renderer *layman_renderer_create(void) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	glClearColor(0, 0, 0, 1); // Black.

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	free(renderer);
}

void layman_renderer_clear(struct layman_renderer *renderer) {
	(void) renderer; // Unused.
	glClear(GL_COLOR_BUFFER_BIT);
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_scene *scene) {
	(void) renderer; // Unused.

	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct layman_entity *entity = scene->entities[i];

		for (size_t j = 0; j < entity->model->meshes_count; j++) {
			struct layman_mesh *mesh = entity->model->meshes[j];

			layman_mesh_use(mesh);
			size_t indices_count = layman_mesh_indices_count(mesh);
			glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, NULL);
			layman_mesh_unuse(mesh);
		}
	}
}
#include "layman.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	int exit_code = EXIT_SUCCESS;

	struct layman_window *window = NULL;
	struct layman_renderer *renderer = NULL;
	struct layman_scene *scene = NULL;
	struct layman_model *model = NULL;
	struct layman_entity *entity = NULL;

	window = layman_window_create(1280, 720, "Example");
	if (!window) {
		fprintf(stderr, "Unable to create the window\n");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	renderer = layman_renderer_create(window);
	if (!renderer) {
		fprintf(stderr, "Unable to create the renderer\n");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	scene = layman_scene_create();
	if (!scene) {
		fprintf(stderr, "Unable to create the scene\n");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	layman_window_use(window);

	model = layman_model_load("C:\\Users\\nitrix\\Desktop\\Stuff\\DamagedHelmet.glb");
	if (!model) {
		fprintf(stderr, "Unable to load the model\n");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	entity = layman_entity_create_from_model(model);
	if (!entity) {
		fprintf(stderr, "Unable to create an entity\n");
		exit_code = EXIT_FAILURE;
		goto cleanup;
	}

	layman_scene_add_entity(scene, entity);

	while (!layman_window_closed(window)) {
		layman_window_poll_events(window, NULL);
		layman_renderer_render(renderer, scene);
		layman_window_refresh(window);
	}

	layman_window_unuse(window);

cleanup:
	if (entity) layman_entity_destroy(entity);
	if (model) layman_model_destroy(model);
	if (scene) layman_scene_destroy(scene);
	if (renderer) layman_renderer_destroy(renderer);
	if (window) layman_window_destroy(window);

	return exit_code;
}
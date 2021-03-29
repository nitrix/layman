#include "camera.h"
#include "environment.h"
#include "renderer.h"
#include "scene.h"
#include "window.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct state {
	struct window *window;
	struct renderer *renderer;
	struct camera *camera;
	struct scene *scene;
	struct environment *environment;
};

struct state state = {
	.window = NULL,
	.renderer = NULL,
};

bool setup() {
	state.window = window_create(1280, 720, "Example", false);
	if (!state.window) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	state.renderer = renderer_create(state.window);
	if (!state.renderer) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	state.camera = camera_create();
	if (!state.camera) {
		fprintf(stderr, "Unable to create the camera\n");
		return false;
	}

	camera_translation(state.camera, 0, 0, 3);

	state.scene = scene_create();
	if (!state.scene) {
		fprintf(stderr, "Unable to create the scene\n");
		return false;
	}

	// state.environment = environment_create_from_hdr("assets/field.hdr");
	// state.environment = environment_create_from_hdr("assets/neutral.hdr");
	state.environment = environment_create_from_hdr("assets/pisa.hdr");
	if (!state.environment) {
		fprintf(stderr, "Unable to create the environment\n");
		return false;
	}

	scene_assign_environment(state.scene, state.environment);

	return true;
}

void cleanup(void) {
	window_destroy(state.window);
	renderer_destroy(state.renderer);
	camera_destroy(state.camera);
	scene_destroy(state.scene);
}

void main_loop(void) {
	while (!window_closed(state.window)) {
		window_poll_events(state.window);

		double elapsed = window_elapsed(state.window);
		double angle = 3.1416 * 0.1f * elapsed;
		// double angle = 0;
		// Moving the camera position around the model, makes the skybox reflection wiggle nicely.
		camera_translation(state.camera, -3 * sinf(angle), 0, 3 * cosf(angle));
		// camera_rotation(state.camera, 0, 3.1416 * 0.1f * elapsed, 0);

		renderer_render(state.renderer, state.camera, state.scene);
	}
}

int main(void) {
	if (!setup()) {
		cleanup();
		return EXIT_FAILURE;
	}

	// TODO: Load model by name, model manager please?

	struct model *model = NULL;
	struct entity *entity = NULL;
	struct light *light = NULL;

	do {
		// model = model_load("assets/BoomBox.glb");
		// model = model_load("assets/DamagedHelmet_Tangents.glb");
		model = model_load("assets/DamagedHelmet.glb");
		if (!model) {
			fprintf(stderr, "Unable to load model\n");
			break;
		}

		entity = entity_create_from_model(model);
		if (!entity) {
			fprintf(stderr, "Unable to create entity\n");
			break;
		}

		light = light_create(LIGHT_TYPE_DIRECTIONAL);
		if (!light) {
			fprintf(stderr, "Unable to create light\n");
			break;
		}

		scene_add_entity(state.scene, entity);
		scene_add_light(state.scene, light);

		main_loop();
	} while (false);

	light_destroy(light);
	entity_destroy(entity);
	model_destroy(model);

	cleanup();

	return EXIT_SUCCESS;
}

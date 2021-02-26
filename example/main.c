#include "layman.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct state {
	struct layman_window *window;
	struct layman_renderer *renderer;
	struct layman_camera *camera;
	struct layman_scene *scene;
	struct layman_environment *environment;
};

struct state state = {
	.window = NULL,
	.renderer = NULL,
};

bool setup() {
	state.window = layman_window_create(1280, 720, "Example", false);
	if (!state.window) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	state.renderer = layman_renderer_create(state.window);
	if (!state.renderer) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	state.camera = layman_camera_create();
	if (!state.camera) {
		fprintf(stderr, "Unable to create the camera\n");
		return false;
	}

	layman_camera_translation(state.camera, 0, 0, 3);

	state.scene = layman_scene_create();
	if (!state.scene) {
		fprintf(stderr, "Unable to create the scene\n");
		return false;
	}

	// state.environment = layman_environment_create_from_hdr(state.window, "field.hdr");
	state.environment = layman_environment_create_from_hdr(state.window, "pisa.hdr");
	// state.environment = layman_environment_create_from_hdr(state.window, "neutral.hdr");
	if (!state.environment) {
		fprintf(stderr, "Unable to create the environment\n");
		return false;
	}

	layman_scene_assign_environment(state.scene, state.environment);

	return true;
}

void cleanup(void) {
	layman_window_destroy(state.window);
	layman_renderer_destroy(state.renderer);
	layman_camera_destroy(state.camera);
	layman_scene_destroy(state.scene);
}

void debug_fps(void) {
	static int fps = 0;
	static double previous = 0;

	fps++;
	double current = layman_window_elapsed(state.window);

	if (current > previous + 1) {
		printf("FPS: %d\n", fps);
		previous = current;
		fps = 0;
	}
}

void main_loop(void) {
	while (!layman_window_closed(state.window)) {
		layman_window_poll_events(state.window);

		double elapsed = layman_window_elapsed(state.window);
		double angle = 3.1416 * 0.1f * elapsed;
		// double angle = 0;
		// This is what we worked on . Moving the camera position around the model, makes the skybox reflection wiggle nicely.
		layman_camera_translation(state.camera, -3 * sinf(angle), 0, 3 * cosf(angle));
		// layman_camera_rotation(state.camera, 0, 3.1416 * 0.1f * elapsed, 0);

		layman_renderer_render(state.renderer, state.camera, state.scene);

		debug_fps();
	}
}

int main(void) {
	if (!setup()) {
		cleanup();
		return EXIT_FAILURE;
	}

	// TODO: Load model by name, model manager please?

	struct layman_model *model = NULL;
	struct layman_entity *entity = NULL;
	struct layman_light *light = NULL;

	do {
		// model = layman_model_load("BoomBox.glb");
		model = layman_model_load(state.window, "DamagedHelmet.glb");
		// model = layman_model_load("DamagedHelmet_Tangents.glb");
		if (!model) {
			fprintf(stderr, "Unable to load model\n");
			break;
		}

		entity = layman_entity_create_from_model(model);
		if (!entity) {
			fprintf(stderr, "Unable to create entity\n");
			break;
		}

		light = layman_light_create(LAYMAN_LIGHT_TYPE_DIRECTIONAL);
		if (!light) {
			fprintf(stderr, "Unable to create light\n");
			break;
		}

		layman_scene_add_entity(state.scene, entity);
		layman_scene_add_light(state.scene, light);

		main_loop();
	} while (false);

	layman_light_destroy(light);
	layman_entity_destroy(entity);
	layman_model_destroy(model);

	cleanup();

	return EXIT_SUCCESS;
}

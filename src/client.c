#include "client.h"

struct client client;

bool setup(void) {
	if (!window_init(&client.window, 1280, 720, DEFAULT_TITLE, false)) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	renderer_init(&client.renderer);
	camera_init(&client.camera);
	scene_init(&client.scene);
	ui_init(&client.ui);
	client.moving = 0;

	client.camera.eye_distance = 3;
	camera_update(&client.camera);

	struct environment *pisa = malloc(sizeof *pisa);
	if (!environment_init_from_file(pisa, "assets/pisa.hdr")) {
		return false;
	}

	client.scene.environment = pisa;

	return true;
}

void cleanup(void) {
	environment_fini(client.scene.environment);

	ui_fini(&client.ui);
	scene_fini(&client.scene);
	window_fini(&client.window);
	renderer_fini(&client.renderer);
}

void main_loop(void) {
	while (!window_closed(&client.window)) {
		window_poll_events(&client.window);

		double elapsed = window_elapsed(&client.window);

		// Orbit camera.
		// Moving the camera position around the model, makes the skybox reflection wiggle nicely.

		float movement_speed = 10.0f;

		if ((client.moving & FORWARD) == FORWARD) {
			camera_center_move_relative(&client.camera, -1 * elapsed * movement_speed, 0);
		}

		if ((client.moving & BACKWARD) == BACKWARD) {
			camera_center_move_relative(&client.camera, elapsed * movement_speed, 0);
		}

		if ((client.moving & LEFT) == LEFT) {
			camera_center_move_relative(&client.camera, 0, -1 * elapsed * movement_speed);
		}

		if ((client.moving & RIGHT) == RIGHT) {
			camera_center_move_relative(&client.camera, 0, elapsed * movement_speed);
		}

		renderer_render(&client.renderer, &client.camera, &client.scene);
		ui_render(&client.ui);
		window_refresh(&client.window);
	}
}

int client_run(void) {
	if (!setup()) {
		cleanup();
		return EXIT_FAILURE;
	}

	do {
		struct light light;
		light_init(&light, LIGHT_TYPE_DIRECTIONAL);
		light.position[2] = -3;

		// scene_add_light(&state.scene, &light);

		main_loop();
	} while (false);

	cleanup();

	return EXIT_SUCCESS;
}

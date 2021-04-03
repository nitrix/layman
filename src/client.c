#include "client.h"
#include "entity.h"
#include "environment.h"
#include "light.h"
#include "model.h"
#include "utils.h"
#include <stdlib.h>

struct client client;

// TODO: Isolate the GLFW calls in here to the window TU.

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(scancode);
	UNUSED(mods);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		client.ui.show = !client.ui.show;
	}
}

void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
	UNUSED(window);

	client.renderer.viewport_width = width;
	client.renderer.viewport_height = height;
	glViewport(0, 0, width, height);
}

bool setup(void) {
	if (!window_init(&client.window, 1280, 720, "Example", false)) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	// Set our callbacks before the UI add theirs on top.
	glfwSetKeyCallback(client.window.glfw_window, key_callback);
	glfwSetFramebufferSizeCallback(client.window.glfw_window, framebuffer_resize_callback);

	renderer_init(&client.renderer);
	camera_init(&client.camera);
	scene_init(&client.scene);
	ui_init(&client.ui);

	camera_translation(&client.camera, 0, 0, 3);

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

		// double elapsed = window_elapsed(state.window);
		// double angle = 3.1416 * 0.1f * elapsed;
		// double angle = 0;

		// Orbit camera.
		// Moving the camera position around the model, makes the skybox reflection wiggle nicely.
		// camera_translation(state.camera, -3 * sinf(angle), 0, 3 * cosf(angle));

		// camera_rotation(state.camera, 0, 3.1416 * 0.1f * elapsed, 0);

		renderer_render(&client.renderer, &client.camera, &client.scene);
		ui_render(&client.ui);
		window_refresh(&client.window);
	}
}

int main(void) {
	if (!setup()) {
		cleanup();
		return EXIT_FAILURE;
	}

	// TODO: Load model by name, model manager please?

	struct model *model = NULL;
	struct entity entity1;
	struct entity entity2;

	do {
		// model = model_load("assets/BoomBox.glb");
		// model = model_load("assets/DamagedHelmet_Tangents.glb");
		// model = model_load("assets/DamagedHelmet.glb");
		model = model_load("assets/foo.glb");
		// model = model_load("assets/foo2.glb");
		if (!model) {
			fprintf(stderr, "Unable to load model\n");
			break;
		}

		entity_init(&entity1, model);
		entity_init(&entity2, model);

		struct light light;
		light_init(&light, LIGHT_TYPE_DIRECTIONAL);
		light.position[2] = -3;

		scene_add_entity(&client.scene, &entity1);
		scene_add_entity(&client.scene, &entity2);
		// scene_add_light(&state.scene, &light);

		main_loop();
	} while (false);

	model_destroy(model);

	cleanup();

	return EXIT_SUCCESS;
}
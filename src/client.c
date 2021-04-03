#include "client.h"

// TODO: Isolate the GLFW calls in here to the window TU.

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(scancode);
	UNUSED(mods);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		state.ui.show = !state.ui.show;
	}
}

void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
	UNUSED(window);

	state.renderer->viewport_width = width;
	state.renderer->viewport_height = height;
	glViewport(0, 0, width, height);
}

bool setup(void) {
	if (!window_init(&state.window, 1280, 720, "Example", false)) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	// Set our callbacks before the UI add theirs on top.
	glfwSetKeyCallback(state.window.glfw_window, key_callback);
	glfwSetFramebufferSizeCallback(state.window.glfw_window, framebuffer_resize_callback);

	state.renderer = renderer_create(&state.window);
	if (!state.renderer) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	camera_init(&state.camera);
	camera_translation(&state.camera, 0, 0, 3);

	scene_init(&state.scene);
	ui_init(&state.ui);

	// state.environment = environment_create_from_hdr("assets/field.hdr");
	// state.environment = environment_create_from_hdr("assets/neutral.hdr");
	state.environment = environment_create_from_hdr("assets/pisa.hdr");
	if (!state.environment) {
		fprintf(stderr, "Unable to create the environment\n");
		return false;
	}

	state.scene.environment = state.environment;

	return true;
}

void cleanup(void) {
	ui_fini(&state.ui);
	scene_fini(&state.scene);
	window_fini(&state.window);
	renderer_destroy(state.renderer);
}

void main_loop(void) {
	while (!window_closed(&state.window)) {
		window_poll_events(&state.window);

		// double elapsed = window_elapsed(state.window);
		// double angle = 3.1416 * 0.1f * elapsed;
		// double angle = 0;

		// Orbit camera.
		// Moving the camera position around the model, makes the skybox reflection wiggle nicely.
		// camera_translation(state.camera, -3 * sinf(angle), 0, 3 * cosf(angle));

		// camera_rotation(state.camera, 0, 3.1416 * 0.1f * elapsed, 0);

		renderer_render(state.renderer, &state.camera, &state.scene);
		ui_render(&state.ui);
		window_refresh(&state.window);
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

		scene_add_entity(&state.scene, &entity1);
		scene_add_entity(&state.scene, &entity2);
		// scene_add_light(&state.scene, &light);

		main_loop();
	} while (false);

	model_destroy(model);

	cleanup();

	return EXIT_SUCCESS;
}

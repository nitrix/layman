#include "GLFW/glfw3.h"
#include "camera.h"
#include "environment.h"
#include "glad/glad.h"
#include "renderer.h"
#include "scene.h"
#include "state.h"
#include "state.h"
#include "ui.h"
#include "utils.h"
#include "window.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(scancode);
	UNUSED(mods);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		state.renderer->ui->show = !state.renderer->ui->show;
	}
}

void resize_callback(GLFWwindow *window, int width, int height) {
	UNUSED(window);

	state.renderer->viewport_width = width;
	state.renderer->viewport_height = height;
	glViewport(0, 0, width, height);
}

bool setup(void) {
	state.window = window_create(1280, 720, "Example", false);
	if (!state.window) {
		fprintf(stderr, "Unable to create the window\n");
		return false;
	}

	// Set our callbacks before the UI add theirs on top.
	glfwSetKeyCallback(state.window->glfw_window, key_callback);
	glfwSetWindowSizeCallback(state.window->glfw_window, resize_callback);

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

		// double elapsed = window_elapsed(state.window);
		// double angle = 3.1416 * 0.1f * elapsed;
		// double angle = 0;

		// Orbit camera.
		// Moving the camera position around the model, makes the skybox reflection wiggle nicely.
		// camera_translation(state.camera, -3 * sinf(angle), 0, 3 * cosf(angle));

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
	struct entity *entity1 = NULL;
	struct entity *entity2 = NULL;
	struct light *light = NULL;

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

		entity1 = entity_create_from_model(model);
		if (!entity1) {
			fprintf(stderr, "Unable to create entity1\n");
			break;
		}

		entity2 = entity_create_from_model(model);
		if (!entity2) {
			fprintf(stderr, "Unable to create entity2\n");
			break;
		}

		light = light_create(LIGHT_TYPE_DIRECTIONAL);
		if (!light) {
			fprintf(stderr, "Unable to create light\n");
			break;
		}

		light->position[2] = -3;

		scene_add_entity(state.scene, entity1);
		// scene_add_entity(state.scene, entity2);
		scene_add_light(state.scene, light);

		main_loop();
	} while (false);

	light_destroy(light);
	entity_destroy(entity1);
	entity_destroy(entity2);
	model_destroy(model);

	cleanup();

	return EXIT_SUCCESS;
}

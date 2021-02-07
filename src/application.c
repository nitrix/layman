#include "layman.h"

// FIXME: I'm starting to question the usefulness of this abstraction. It's becoming thinner by the second...

struct layman_application *layman_application_create(int width, int height, const char *title, bool fullscreen) {
	struct layman_application *app = malloc(sizeof *app);
	if (!app) {
		return NULL;
	}

	app->window = NULL;
	app->renderer = NULL;
	app->camera = NULL;
	app->scene = NULL;
	app->environment = NULL;

	app->window = layman_window_create(width, height, title, fullscreen);
	if (!app->window) {
		goto failure;
	}

	app->renderer = layman_renderer_create();
	if (!app->renderer) {
		goto failure;
	}

	app->camera = layman_camera_create();
	if (!app->camera) {
		goto failure;
	}

	app->scene = layman_scene_create();
	if (!app->scene) {
		goto failure;
	}

	// TODO: No hardcoded filepaths.
	app->environment = layman_environment_create_from_hdr("pisa.hdr");
	if (!app->environment) {
		goto failure;
	}

	layman_scene_assign_environment(app->scene, app->environment);

	return app;

failure:
	layman_application_destroy(app);
	return NULL;
}

void layman_application_destroy(struct layman_application *app) {
	layman_window_destroy(app->window);
	layman_renderer_destroy(app->renderer);
	layman_camera_destroy(app->camera);
	layman_scene_destroy(app->scene);
	layman_environment_destroy(app->environment);

	free(app);
}

void layman_application_run(struct layman_application *app) {
	layman_window_switch(app->window);
	layman_renderer_switch(app->renderer);

	int fps = 0;
	double last_time = glfwGetTime();

	while (!layman_window_closed(app->window)) {
		layman_window_poll_events(app->window);
		layman_renderer_render(app->renderer, app->camera, app->scene);
		layman_window_refresh(app->window);

		fps++;
		double current_time = glfwGetTime();

		if (current_time > last_time + 1) {
			printf("FPS: %d\n", fps);
			last_time = current_time;
			fps = 0;
		}
	}
}

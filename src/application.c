#include "layman.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

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

	app->window = layman_window_create(width, height, title, fullscreen);
	if (!app->window) {
		goto failure;
	}

	layman_window_use(app->window);

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

	layman_window_unuse(app->window);

	return app;

failure:
	layman_application_destroy(app);
	return NULL;
}

void layman_application_destroy(struct layman_application *app) {
	if (app->window) {
		layman_window_destroy(app->window);
	}

	if (app->renderer) {
		layman_renderer_destroy(app->renderer);
	}

	if (app->camera) {
		layman_camera_destroy(app->camera);
	}

	if (app->scene) {
		layman_scene_destroy(app->scene);
	}

	free(app);
}

void layman_application_use(struct layman_application *app) {
	layman_window_use(app->window);
}

void layman_application_unuse(struct layman_application *app) {
	layman_window_unuse(app->window);
}

void layman_application_run(struct layman_application *app) {
	layman_renderer_use(app->renderer);

	int fps = 0;
	double last_time = glfwGetTime();

	while (!layman_window_closed(app->window)) {
		layman_window_poll_events(app->window, NULL);
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

	layman_renderer_unuse(app->renderer);
}

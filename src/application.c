#include "layman.h"

struct layman_application *layman_application_create(int width, int height, const char *title) {
	struct layman_application *app = malloc(sizeof *app);
	if (!app) {
		return NULL;
	}

	app->window = NULL;
	app->renderer = NULL;
	app->scene = NULL;
	app->shader = NULL;

	app->window = layman_window_create(width, height, title);
	if (!app->window) goto failure;

	layman_window_use(app->window);

	app->renderer = layman_renderer_create();
	if (!app->renderer) goto failure;

	app->scene = layman_scene_create();
	if (!app->scene) goto failure;

	// TODO: Embedding.
	app->shader = layman_shader_load_from_file("shaders/pbr.vert", "shaders/pbr.frag");
	if (!app->shader) goto failure;

	layman_window_unuse(app->window);

	return app;

failure:
	layman_application_destroy(app);
	return NULL;
}

void layman_application_destroy(struct layman_application *app) {
	if (app->window) layman_window_destroy(app->window);
	if (app->renderer) layman_renderer_destroy(app->renderer);
	if (app->scene) layman_scene_destroy(app->scene);
	if (app->shader) layman_shader_destroy(app->shader);

	free(app);
}

void layman_application_use(struct layman_application *app) {
	layman_window_use(app->window);
	layman_shader_use(app->shader);
}

void layman_application_unuse(struct layman_application *app) {
	layman_shader_unuse(app->shader);
	layman_window_unuse(app->window);
}

void layman_application_run(struct layman_application *app) {
	while (!layman_window_closed(app->window)) {
		layman_window_poll_events(app->window, NULL);
		layman_renderer_render(app->renderer, app->scene);
		layman_window_refresh(app->window);
	}
}
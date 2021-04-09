#ifndef WINDOW_H
#define WINDOW_H

#include "GLFW/glfw3.h"
#include <stdbool.h>

struct window {
	GLFWwindow *glfw_window;
	unsigned int width, height;
	int samples;
	bool fullscreen;
	char *title;

	double last_time, now_time;

	double cursor_pos_x, cursor_pos_y;
	vec4 cursor_ray_origin, cursor_ray_direction;
};

bool window_init(struct window *window, unsigned int width, unsigned int height, const char *title, bool fullscreen);
void window_fini(struct window *window);
void window_close(struct window *window, int force);
bool window_closed(const struct window *window);
void window_poll_events(struct window *window);
double window_elapsed(const struct window *window);
void window_framebuffer_size(const struct window *window, int *width, int *height);
void window_fullscreen(struct window *window, bool fullscreen);
bool window_extension_supported(const char *name);
void window_refresh(struct window *window);
void window_update_title(struct window *window, const char *title);

#endif

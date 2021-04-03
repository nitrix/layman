#ifndef WINDOW_H
#define WINDOW_H

struct window {
	GLFWwindow *glfw_window;
	unsigned int width, height;
	double start_time;
	int samples;
	bool fullscreen;
	double cursor_pos_x, cursor_pos_y;
};

bool window_init(struct window *window, unsigned int width, unsigned int height, const char *title, bool fullscreen);
void window_fini(struct window *window);
void window_close(struct window *window, int force);
bool window_closed(const struct window *window);
void window_poll_events(const struct window *window);
double window_elapsed(const struct window *window);
void window_framebuffer_size(const struct window *window, int *width, int *height);
void window_fullscreen(struct window *window, bool fullscreen);
bool window_extension_supported(const char *name);
void window_refresh(const struct window *window);

#endif

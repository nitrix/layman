#ifndef LEARNGL_WINDOW_H
#define LEARNGL_WINDOW_H

#include "toolkit.h"

struct window;
struct entity;

struct window *window_create(int width, int height, const char *title);
void window_destroy(struct window *window);

// Usage
void window_use(const struct window *window);
void window_refresh(const struct window *window);
void window_unuse(const struct window *window);

// Utilities
float window_elapsed_seconds(struct window *window);
void window_framebuffer_size(struct window *window, int *width, int *height);

// Closing
void window_close(struct window *window);
bool window_should_close(const struct window *window);

// Events
tk_result window_poll_event(struct window *window);
bool window_event_key_pressed(const struct window *window, char key);
bool window_event_key_released(const struct window *window, char key);
bool window_event_mouse_button_pressed(const struct window *window, int button);
bool window_event_mouse_button_released(const struct window *window, int button);
bool window_event_mouse_motion_relative(const struct window *window, int *delta_x, int *delta_y);
bool window_event_mouse_wheel(const struct window *window, int *delta_x, int *delta_y);

#endif

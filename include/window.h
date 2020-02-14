#ifndef LEARNGL_WINDOW_H
#define LEARNGL_WINDOW_H

#include "toolkit.h"
#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct window;

enum window_action {
    WINDOW_ACTION_PRESS   = 1,
    WINDOW_ACTION_RELEASE = 2,
    WINDOW_ACTION_REPEAT  = 3,
};

typedef void (window_on_key_func)(struct window *window, int key, enum window_action action);
typedef void (window_on_mouse_button_func)(struct window *window, int button, enum window_action action);
typedef void (window_on_mouse_position_func)(struct window *window, double x, double y);
typedef void (window_on_mouse_wheel_func)(struct window *window, double delta);

struct window {
    GLFWwindow *glfw_window;
    window_on_key_func *on_key_func;
    window_on_mouse_button_func *on_mouse_button_func;
    window_on_mouse_position_func *on_mouse_position_func;
    window_on_mouse_wheel_func *on_mouse_wheel_func;
    void *custom;
};

struct window *window_create(int width, int height, const char *title);
void window_destroy(struct window *window);

// Usage
void window_use(const struct window *window);
void window_refresh(const struct window *window);
void window_unuse(const struct window *window);
// Utilities
double window_elapsed_seconds(const struct window *window);
void window_framebuffer_size(const struct window *window, int *width, int *height);
// Closing
void window_close(const struct window *window);
bool window_should_close(const struct window *window);
// Custom pointer
void *window_custom(const struct window *window);
void window_set_custom(struct window *window, void *ptr);
// Event polling
void window_poll_events(const struct window *window);
// Key events
void window_set_key_callback(struct window *window, window_on_key_func *on_key_func);
// Mouse button events
void window_set_mouse_button_callback(struct window *window, window_on_mouse_button_func *on_mouse_button_func);
// Mouse position events
void window_set_mouse_position_callback(struct window *window, window_on_mouse_position_func *on_mouse_position_func);
// Mouse wheel events
void window_set_mouse_wheel_callback(struct window *window, window_on_mouse_wheel_func *on_mouse_wheel_func);

#endif

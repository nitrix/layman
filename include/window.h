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
enum window_key_action {
    WINDOW_KEY_ACTION_PRESS   = 1,
    WINDOW_KEY_ACTION_RELEASE = 2,
    WINDOW_KEY_ACTION_REPEAT  = 3,
};
typedef void (window_on_key_func)(struct window *window, int key, enum window_key_action action);
void window_set_key_callback(struct window *window, window_on_key_func *on_key_func);

// Mouse events

#endif

#ifndef LEARNGL_WINDOW_H
#define LEARNGL_WINDOW_H

#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct window {
    GLFWwindow *glfw_window;
    int width;
    int height;
};

void window_init(void);
void window_fini(void);

struct window *window_create(int width, int height, const char *title);
void window_destroy(struct window *window);

void window_switch_context(struct window *window);
void window_framebuffer_size(struct window *window, int *width, int *height);
void window_refresh(struct window *window);
bool window_should_close(struct window *window);

#endif
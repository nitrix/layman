#include <stdlib.h>
#include "window.h"

void window_init() {
    glfwInit();

    // OpenGL 3.1 removed the immediate mode stuff.
    // OpenGL 3.3, the shader versions starts matching the gl version.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
}

void window_fini() {
    glfwTerminate();
}

struct window *window_create(int width, int height, const char *title) {
    struct window *window = malloc(sizeof *window);
    if (!window) {
        return NULL;
    }

    window->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window->glfw_window) {
        free(window);
        return NULL;
    }

    return window;
}

void window_framebuffer_size(struct window *window, int *width, int *height) {
    glfwGetFramebufferSize(window->glfw_window, width, height);
}

void window_switch_context(struct window *window) {
    // Detach the current context (on the old thread)
    glfwMakeContextCurrent(NULL);

    // Attach the current context (on the new thread)
    glfwMakeContextCurrent(window->glfw_window);
}

void window_refresh(struct window *window) {
    glfwSwapBuffers(window->glfw_window);
}

bool window_should_close(struct window *window) {
    return glfwWindowShouldClose(window->glfw_window);
}

void window_destroy(struct window *window) {
    glfwDestroyWindow(window->glfw_window);
    free(window);
}
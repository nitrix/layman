#include "toolkit.h"
#include "camera.h"

#include <GLFW/glfw3.h>

struct window {
    GLFWwindow *glfw_window;
};

atomic_uint window_count;

void window_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&window_count, 1);

    if (previous_count == 0) {
        glfwInit();

        // OpenGL 3.1 removed the immediate mode stuff.
        // OpenGL 3.3, the shader versions starts matching the gl version.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }
}

void window_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&window_count, 1);

    if (previous_count == 1) {
        glfwTerminate();
    }
}

struct window *window_create(int width, int height, const char *title) {
    window_increment();

    struct window *window = malloc(sizeof *window);
    if (!window) {
        window_decrement();
        return NULL;
    }

    window->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window->glfw_window) {
        free(window);
        window_decrement();
        return NULL;
    }

    return window;
}

void window_destroy(struct window *window) {
    glfwDestroyWindow(window->glfw_window);
    free(window);
    window_decrement();
}

void window_handle_events(struct window *window, struct camera *camera) {
    glfwPollEvents();

    if (glfwGetKey(window->glfw_window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_move(camera, 0, 0, -0.02f);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_move(camera, 0, 0, 0.02f);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_move(camera, -0.02f, 0, 0);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_move(camera, 0.02f, 0, 0);
    }
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

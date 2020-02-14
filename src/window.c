#include "window.h"

atomic_uint window_refcount;

void window_refcount_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&window_refcount, 1);

    if (previous_count == 0) {
        glfwInit();
    }
}

void window_refcount_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&window_refcount, 1);

    if (previous_count == 1) {
        glfwTerminate();
    }
}

struct window *window_create(int width, int height, const char *title) {
    struct window *window = malloc(sizeof *window);
    if (!window) {
        return NULL;
    }

    window_refcount_increment();

    // This will give us a context for the highest version of OpenGL greater than or equal to 3.2 that is supported by
    // the OS and GPU. This works because no features were deprecated between OpenGL 3.2 and 4.1, making
    // forward-compatible OpenGL 4.1 backwards-compatible with 3.2. If the requested version is not supported, window
    // creation will fail. The only OpenGL 3.x and 4.x contexts currently supported by macOS are forward-compatible,
    // core profile contexts (3.2 on 10.7 Lion, 3.3 or 4.1 on 10.9 Mavericks).
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = NULL; // Windowed mode
    window->glfw_window = glfwCreateWindow(width, height, title, monitor, NULL);
    if (!window->glfw_window) {
        free(window);
        window_refcount_decrement();
        return NULL;
    }

    glfwSetWindowUserPointer(window->glfw_window, window);
    glfwSetTime(0);

    return window;
}

void window_use(const struct window *window) {
    glfwMakeContextCurrent(window->glfw_window);
}

void window_unuse(const struct window *window) {
    TK_UNUSED(window);
    glfwMakeContextCurrent(NULL);
}

void window_destroy(struct window *window) {
    glfwDestroyWindow(window->glfw_window);
    free(window);
    window_refcount_decrement();
}

void window_poll_events(const struct window *window) {
    TK_UNUSED(window);

    glfwPollEvents();
}

enum window_action window_convert_glfw_action(int action) {
    switch (action) {
        case GLFW_PRESS:
            return WINDOW_ACTION_PRESS;
        case GLFW_RELEASE:
            return WINDOW_ACTION_RELEASE;
        case GLFW_REPEAT:
            return WINDOW_ACTION_REPEAT;
        default:
            return 0;
    }
}

void window_key_relay(GLFWwindow *glfw_window, int key, int scan_code, int action, int mods) {
    TK_UNUSED(scan_code);
    TK_UNUSED(mods);

    struct window *window = glfwGetWindowUserPointer(glfw_window);
    enum window_action converted_action = window_convert_glfw_action(action);

    window->on_key_func(window, key, converted_action);
}

void window_set_key_callback(struct window *window, window_on_key_func *on_key_func) {
    window->on_key_func = on_key_func;
    glfwSetKeyCallback(window->glfw_window, window_key_relay);
}

void window_mouse_button_relay(GLFWwindow *glfw_window, int button, int action, int mods) {
    struct window *window = glfwGetWindowUserPointer(glfw_window);

    enum window_action converted_action = window_convert_glfw_action(action);
    window->on_mouse_button_func(window, button, converted_action);
}

void window_set_mouse_button_callback(struct window *window, window_on_mouse_button_func *on_mouse_button_func) {
    window->on_mouse_button_func = on_mouse_button_func;
    glfwSetMouseButtonCallback(window->glfw_window, window_mouse_button_relay);
}

void window_mouse_position_relay(GLFWwindow *glfw_window, double x, double y) {
    struct window *window = glfwGetWindowUserPointer(glfw_window);

    window->on_mouse_position_func(window, x, y);
}

void window_set_mouse_position_callback(struct window *window, window_on_mouse_position_func *on_mouse_position_func) {
    window->on_mouse_position_func = on_mouse_position_func;
    glfwSetCursorPosCallback(window->glfw_window, window_mouse_position_relay);
}

void window_mouse_wheel_relay(GLFWwindow *glfw_window, double delta_x, double delta_y) {
    TK_UNUSED(delta_x);

    struct window *window = glfwGetWindowUserPointer(glfw_window);

    window->on_mouse_wheel_func(window, delta_y);
}

void window_set_mouse_wheel_callback(struct window *window, window_on_mouse_wheel_func *on_mouse_wheel_func) {
    window->on_mouse_wheel_func = on_mouse_wheel_func;
    glfwSetScrollCallback(window->glfw_window, window_mouse_wheel_relay);
}

double window_elapsed_seconds(const struct window *window) {
    TK_UNUSED(window);

    double elapsed_seconds = glfwGetTime();
    glfwSetTime(0);
    return elapsed_seconds;
}

void window_framebuffer_size(const struct window *window, int *width, int *height) {
    glfwGetFramebufferSize(window->glfw_window, width, height);
}

void window_refresh(const struct window *window) {
    glfwSwapBuffers(window->glfw_window);
}

bool window_should_close(const struct window *window) {
    return glfwWindowShouldClose(window->glfw_window);
}

void window_close(const struct window *window) {
    glfwSetWindowShouldClose(window->glfw_window, GLFW_TRUE);
}

void *window_custom(const struct window *window) {
    return window->custom;
}

void window_set_custom(struct window *window, void *ptr) {
    window->custom = ptr;
}

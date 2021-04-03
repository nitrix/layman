#include "client.h"

void apply_fallback_resolution(unsigned int *width, unsigned int *height) {
	GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
	if (!primary_monitor) {
		return;
	}

	const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);
	if (!video_mode) {
		return;
	}

	if (*width == 0) {
		*width = video_mode->width;
	}

	if (*height == 0) {
		*height = video_mode->height;
	}
}

void window_fullscreen(struct window *window, bool fullscreen) {
	static int original_width, original_height, original_x, original_y;

	if (fullscreen) {
		glfwGetWindowSize(window->glfw_window, &original_width, &original_height);
		glfwGetWindowPos(window->glfw_window, &original_x, &original_y);

		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window->glfw_window, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
	} else {
		glfwSetWindowMonitor(window->glfw_window, NULL, original_x, original_y, original_width, original_height, 0);
	}

	window->fullscreen = fullscreen;
}

struct window *window_create(unsigned int width, unsigned int height, const char *title, bool fullscreen) {
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}

	window->width = width;
	window->height = height;
	window->start_time = glfwGetTime();
	window->samples = 4; // TODO: Support changing the number of samples. This requires recreating the window and sharing the context.
	window->fullscreen = fullscreen;

	// Automatically initializes the GLFW library for the first window created.
	if (glfwInit() == GLFW_FALSE) {
		free(window);
		return NULL;
	}

	// Use the primary monitor's resolution when dimensions weren't specified.
	apply_fallback_resolution(&window->width, &window->height);

	// GLFW window hints.
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, window->samples); // Multisampling.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Mac only supports OpenGL 3.2 or 4.1.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern rendering pipeline.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true); // Mac OS X requires forward compatibility.
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, wants_debugging());

	// Fullscreen mode always uses the primary monitor for now.
	GLFWmonitor *monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;

	// Create the actual window using the GLFW library.
	window->glfw_window = glfwCreateWindow(window->width, window->height, title, monitor, NULL);
	if (!window->glfw_window) {
		free(window);
		glfwTerminate();
		return NULL;
	}

	// Make the current thread use the new window's OpenGL context so that we can initialize OpenGL for it.
	glfwMakeContextCurrent(window->glfw_window);

	// Initialize OpenGL.
	if (!gladLoadGL()) {
		glfwDestroyWindow(window->glfw_window);
		free(window);
		glfwTerminate();
		return NULL;
	}

	// Setup OpenGL debugging.
	// setup_opengl_debugging();

	// Minimum number of monitor refreshes the driver should wait after the call to glfwSwapBuffers before actually swapping the buffers on the display.
	// Essentially, 0 = V-Sync off, 1 = V-Sync on. Leaving this on avoids ugly tearing artifacts.
	// It requires the OpenGL context to be effective on Windows.
	glfwSwapInterval(1);

	return window;
}

double window_elapsed(const struct window *window) {
	return glfwGetTime() - window->start_time;
}

void window_destroy(struct window *window) {
	if (!window) {
		return;
	}

	free(window);
	glfwTerminate();
}

void window_close(const struct window *window) {
	glfwSetWindowShouldClose(window->glfw_window, 1);
}

bool window_closed(const struct window *window) {
	return glfwWindowShouldClose(window->glfw_window) == 1;
}

void window_poll_events(const struct window *window) {
	UNUSED(window);
	glfwPollEvents();
}

void window_framebuffer_size(const struct window *window, unsigned int *width, unsigned int *height) {
	int tmp_width, tmp_height;
	glfwGetFramebufferSize(window->glfw_window, &tmp_width, &tmp_height);
	*width = tmp_width;
	*height = tmp_height;
}

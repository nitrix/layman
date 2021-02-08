#include "layman.h"

thread_local const struct layman_window *current_window;

// This is a reference count of windows to abstract away the initialization/termination of the GLFW library.
// The first window created will automatically initialize the library, while the last window destroyed will automatically terminate it.
// We don't have to worry about concurrency here, since layman_window_create() and layman_window_destroy() can only be used from the main thread.
static int refcount = 0;

static bool increment_refcount(void) {
	if (refcount == 0) {
		if (glfwInit() == GLFW_FALSE) {
			return false;
		}
	}

	refcount++;

	return true;
}

static void decrement_refcount(void) {
	refcount--;

	if (refcount == 0) {
		glfwTerminate();
	}
}

struct layman_window *layman_window_create(int width, int height, const char *title, bool fullscreen) {
	struct layman_window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}

	// Automatically initializes the GLFW library for the first window created.
	if (!increment_refcount()) {
		free(window);
		return NULL;
	}

	// Use the primary monitor's resolution when dimensions weren't specified.
	GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);

	if (width <= 0) {
		width = video_mode->width;
	}

	if (height <= 0) {
		height = video_mode->height;
	}

	// Create the actual window using the GLFW library.

	glfwWindowHint(GLFW_VISIBLE, false);
	glfwWindowHint(GLFW_DECORATED, true);
	glfwWindowHint(GLFW_FOCUSED, true);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, 4); // Multisampling.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern rendering pipeline.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true); // Mac OS X requires forward compatibility.

	window->glfw_window = glfwCreateWindow(width, height, title, fullscreen ? primary_monitor : NULL, NULL);
	if (!window->glfw_window) {
		free(window);
		decrement_refcount();
		return NULL;
	}

	// Make the current thread use the new window's OpenGL context so that we can initialize OpenGL for it.
	layman_window_switch(window);

	// Initialize OpenGL.
	if (!gladLoadGL()) {
		glfwDestroyWindow(window->glfw_window);
		free(window);
		decrement_refcount();
		return NULL;
	}

	// Minimum number of monitor refreshes the driver should wait after the call to glfwSwapBuffers before actually swapping the buffers on the display.
	// Essentially, 0 = V-Sync off, 1 = V-Sync on. Leaving this on avoids ugly tearing artifacts.
	// It requires the OpenGL context to be effective on Windows.
	glfwSwapInterval(1);

	// Center the window.
	glfwSetWindowPos(window->glfw_window, video_mode->width / 2 - width / 2, video_mode->height / 2 - height / 2);

	// Show the window.
	// This is the last thing we want to do, to make sure the user doesn't see the setup as it happens.
	glfwShowWindow(window->glfw_window);

	return window;
}

void layman_window_destroy(struct layman_window *window) {
	if (!window) {
		return;
	}

	free(window);
	decrement_refcount();
}

void layman_window_close(const struct layman_window *window) {
	glfwSetWindowShouldClose(window->glfw_window, 1);
}

bool layman_window_closed(const struct layman_window *window) {
	return glfwWindowShouldClose(window->glfw_window) == 1;
}

void layman_window_switch(const struct layman_window *window) {
	if (current_window == window) {
		return;
	} else {
		current_window = window;
	}

	if (window) {
		glfwMakeContextCurrent(window->glfw_window);
	}
}

void layman_window_poll_events(const struct layman_window *window) {
	UNUSED(window);
	glfwPollEvents();
}

void layman_window_refresh(const struct layman_window *window) {
	layman_window_switch(window);
	glfwSwapBuffers(window->glfw_window);
}

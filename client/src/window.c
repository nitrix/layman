#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "utils.h"
#include "window.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void opengl_debug_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *custom) {
	UNUSED(length);
	UNUSED(custom);

	fprintf(stderr, "Debug message (%d): %s\n", id, message);

	switch (source) {
	    case GL_DEBUG_SOURCE_API:             fprintf(stderr, "Source: API"); break;
	    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "Source: Window System"); break;
	    case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler"); break;
	    case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "Source: Third Party"); break;
	    case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "Source: Application"); break;
	    case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "Source: Other"); break;
	}

	fprintf(stderr, "\n");

	switch (type) {
	    case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "Type: Error"); break;
	    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour"); break;
	    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "Type: Undefined Behaviour"); break;
	    case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "Type: Portability"); break;
	    case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "Type: Performance"); break;
	    case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "Type: Marker"); break;
	    case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "Type: Push Group"); break;
	    case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "Type: Pop Group"); break;
	    case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "Type: Other"); break;
	}

	fprintf(stderr, "\n");

	switch (severity) {
	    case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity: high"); break;
	    case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity: medium"); break;
	    case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity: low"); break;
	    case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification"); break;
	}

	fprintf(stderr, "\n\n");
}

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

static void setup_opengl_debugging(void) {
	#ifndef OPENGL_DEBUGGING
	return;
	#endif

	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_debug_callback, NULL);

		// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

		GLuint ignored[] = {131169, 131185, 131218, 131204}; // TODO: Document or fix these.
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, ARRAY_COUNT(ignored), ignored, GL_FALSE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, ARRAY_COUNT(ignored), ignored, GL_FALSE);
	}
}

static bool wants_debugging() {
	// Mac doesn't support debugging.
	// They only go up to version 4.1 and debugging requires 4.3 or above.

	#if __APPLE__
	return false;
	#endif

	#if DEBUG
	return true;
	#endif

	return false;
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

	// Automatically initializes the GLFW library for the first window created.
	if (glfwInit() == GLFW_FALSE) {
		free(window);
		return NULL;
	}

	// Use the primary monitor's resolution when dimensions weren't specified.
	apply_fallback_resolution(&window->width, &window->height);

	// Determine the OpenGL version.
	// Use 4.1 on Apple devices and 4.3 everywhere else.
	int major = 4, minor = 3;
	#if __APPLE__
	minor = 1;
	#endif

	// GLFW window hints.
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, window->samples); // Multisampling.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern rendering pipeline.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true); // Mac OS X requires forward compatibility.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, wants_debugging());

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
	GLFWwindow *previous_context = glfwGetCurrentContext();
	glfwMakeContextCurrent(window->glfw_window);

	// Initialize OpenGL.
	if (!gladLoadGL()) {
		glfwDestroyWindow(window->glfw_window);
		free(window);
		glfwTerminate();
		return NULL;
	}

	// Setup OpenGL debugging.
	setup_opengl_debugging();

	// Minimum number of monitor refreshes the driver should wait after the call to glfwSwapBuffers before actually swapping the buffers on the display.
	// Essentially, 0 = V-Sync off, 1 = V-Sync on. Leaving this on avoids ugly tearing artifacts.
	// It requires the OpenGL context to be effective on Windows.
	glfwSwapInterval(1);

	// Restore the previous context.
	glfwMakeContextCurrent(previous_context);

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

void window_use(const struct window *window) {
	glfwMakeContextCurrent(window->glfw_window);
}

void window_unuse(const struct window *window) {
	UNUSED(window);

	// By default, making a context non-current implicitly forces a pipeline flush.
	// On platforms that support `GL_KHR_context_flush_control`, it's possible to control
	// whether a context performs the flush by setting the `GLFW_CONTEXT_RELEASE_BEHAVIOR` window hint.
	glfwMakeContextCurrent(NULL);
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

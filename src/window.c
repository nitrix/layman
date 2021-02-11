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

static void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
	UNUSED(length);
	UNUSED(userParam);

	// Ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

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

struct layman_window *layman_window_create(int width, int height, const char *title, bool fullscreen) {
	struct layman_window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}

	window->width = width;
	window->height = height;

	// Automatically initializes the GLFW library for the first window created.
	if (!increment_refcount()) {
		free(window);
		return NULL;
	}

	// Use the primary monitor's resolution when dimensions weren't specified.
	GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);

	if (window->width <= 0) {
		window->width = video_mode->width;
	}

	if (window->height <= 0) {
		window->height = video_mode->height;
	}

	int major = 4;
	int minor = 3;

	#if LAYGL_DEBUG
	bool debugging = true;
	#else
	bool debugging = false;
	#endif

	// Mac has limitations. I can only go up to version 4.1 and doesn't support debugging (only in 4.3+).
	#if __APPLE__
	major = 4;
	minor = 1;
	debugging = false;
	#endif

	// Create the actual window using the GLFW library.
	glfwWindowHint(GLFW_VISIBLE, false);
	glfwWindowHint(GLFW_DECORATED, true);
	glfwWindowHint(GLFW_FOCUSED, true);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, 4); // Multisampling.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern rendering pipeline.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true); // Mac OS X requires forward compatibility.

	// Enable OpenGL debugging when in debug mode.
	if (debugging) {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}

	window->glfw_window = glfwCreateWindow(window->width, window->height, title, fullscreen ? primary_monitor : NULL, NULL);
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

	// Configure OpenGL debugging when in debug mode.
	if (debugging) {
		GLint context_flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
		if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, NULL);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		}
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

#ifndef LAYMAN_PRIVATE_WINDOW_H
#define LAYMAN_PRIVATE_WINDOW_H

#include "GLFW/glfw3.h"

struct layman_window {
	GLFWwindow *glfw_window;
	unsigned int width;
	unsigned int height;
	double start_time;
	int samples;
};

/**
 * @brief Mark a window for use.
 *
 * The window must be marked for use before OpenGL commands are issued.
 *
 * @remark Every use must be paired with an unuse once the commands are done.
 * @see layman_window_unuse().
 */
void layman_window_use(const struct layman_window *window);
void layman_window_unuse(const struct layman_window *window);

#endif

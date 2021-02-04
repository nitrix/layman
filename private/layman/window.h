#ifndef LAYMAN_PRIVATE_WINDOW_H
#define LAYMAN_PRIVATE_WINDOW_H

#include "GLFW/glfw3.h"

struct layman_window {
	GLFWwindow *glfw_window;
};

void layman_window_switch(const struct layman_window *window);

#endif

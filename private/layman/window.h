#ifndef LAYMAN_PRIVATE_WINDOW_H
#define LAYMAN_PRIVATE_WINDOW_H

struct layman_window {
	GLFWwindow *glfw_window;
	int width, height;
};

void layman_window_switch(const struct layman_window *window);

#endif

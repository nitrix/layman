#ifndef LAYMAN_PRIVATE_FRAMEBUFFER_H
#define LAYMAN_PRIVATE_FRAMEBUFFER_H

#include "glad/glad.h"

struct layman_framebuffer {
	int width, height;
	GLuint fbo;
	GLuint rbo;
};

#endif

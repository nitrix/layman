#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad/glad.h"

struct framebuffer {
	int width, height;
	GLuint fbo;
	GLuint rbo;
};

// TODO: Documentation.
struct framebuffer *framebuffer_create(int width, int height);

// TODO: Documentation.
void framebuffer_destroy(struct framebuffer *fb);

#endif

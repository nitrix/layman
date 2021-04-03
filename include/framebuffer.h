#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad/glad.h"

struct framebuffer {
	int width, height;
	GLuint fbo;
	GLuint rbo;
};

void framebuffer_init(struct framebuffer *fb, int width, int height);
void framebuffer_fini(struct framebuffer *fb);

#endif

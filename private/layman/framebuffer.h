#ifndef LAYMAN_PRIVATE_FRAMEBUFFER_H
#define LAYMAN_PRIVATE_FRAMEBUFFER_H

struct layman_framebuffer {
	int width, height;
	GLuint fbo;
	GLuint rbo;
};

#endif

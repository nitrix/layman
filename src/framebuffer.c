#include "layman.h"

thread_local const struct layman_framebuffer *current_framebuffer;

struct layman_framebuffer *layman_framebuffer_create(int width, int height) {
	struct layman_framebuffer *fb = malloc(sizeof *fb);
	if (!fb) {
		return NULL;
	}

	fb->width = width;
	fb->height = height;

	glGenFramebuffers(1, &fb->fbo);
	glGenRenderbuffers(1, &fb->rbo);

	return fb;
}

void layman_framebuffer_destroy(struct layman_framebuffer *fb) {
	free(fb);
}

void layman_framebuffer_switch(const struct layman_framebuffer *fb) {
	if (current_framebuffer == fb) {
		return;
	} else {
		current_framebuffer = fb;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fb->width, fb->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);
}

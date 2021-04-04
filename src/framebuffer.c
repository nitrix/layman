#include "client.h"

void framebuffer_init(struct framebuffer *fb, int width, int height) {
	fb->width = width;
	fb->height = height;

	glGenFramebuffers(1, &fb->fbo);
	glGenRenderbuffers(1, &fb->rbo);
}

void framebuffer_fini(struct framebuffer *fb) {
	glDeleteRenderbuffers(1, &fb->rbo);
	glDeleteFramebuffers(1, &fb->fbo);
}

void framebuffer_switch(const struct framebuffer *new) {
	glBindFramebuffer(GL_FRAMEBUFFER, new->fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, new->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, new->width, new->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, new->rbo);
}

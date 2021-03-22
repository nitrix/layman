#include "framebuffer.h"
#include "utils.h"
#include <stdlib.h>

struct framebuffer *framebuffer_create(int width, int height) {
	struct framebuffer *fb = malloc(sizeof *fb);
	if (!fb) {
		return NULL;
	}

	fb->width = width;
	fb->height = height;

	glGenFramebuffers(1, &fb->fbo);
	glGenRenderbuffers(1, &fb->rbo);

	return fb;
}

void framebuffer_destroy(struct framebuffer *fb) {
	free(fb);
}

void framebuffer_switch(const struct framebuffer *new) {
	thread_local static const struct framebuffer *current;

	if (new == current) {
		return;
	}

	current = new;

	if (new) {
		glBindFramebuffer(GL_FRAMEBUFFER, new->fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, new->rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, new->width, new->height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, new->rbo);
	}
}

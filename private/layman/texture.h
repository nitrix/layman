#ifndef LAYMAN_PRIVATE_TEXTURE_H
#define LAYMAN_PRIVATE_TEXTURE_H

struct layman_texture {
	size_t width;
	size_t height;
	size_t levels;

	enum layman_texture_kind kind;

	// Object stuff.
	GLuint id;

	// Sampler stuff.
	GLenum gl_target;
	GLenum gl_unit;

	// Pixel data stuff.
	GLenum gl_type;
	GLenum gl_format;
	GLenum gl_internal_format;
};

void layman_texture_switch(const struct layman_texture *texture);

#endif

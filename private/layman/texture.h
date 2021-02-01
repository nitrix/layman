#ifndef LAYMAN_PRIVATE_TEXTURE_H
#define LAYMAN_PRIVATE_TEXTURE_H

#include <glad/glad.h>

struct layman_texture {
	GLuint id;
	enum layman_texture_kind kind;
};

#endif

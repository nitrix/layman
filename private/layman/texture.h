#ifndef LAYMAN_PRIVATE_TEXTURE_H
#define LAYMAN_PRIVATE_TEXTURE_H

#include <glad/glad.h>

struct layman_texture {
	GLuint id;
	enum layman_texture_kind kind;

	size_t width, height;
	size_t levels;

	enum layman_texture_data_type data_type;
	enum layman_texture_data_format data_format;
	enum layman_texture_data_internal_format data_internal_format;
};

#endif

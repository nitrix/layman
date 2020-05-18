#ifndef LEARNGL_TEXTURE_H
#define LEARNGL_TEXTURE_H

#include "shader.h"
#include "toolkit.h"

#include <glad/glad.h>

struct texture {
    GLuint texture_id;
};

struct texture *texture_load(const char *filepath);
void texture_destroy(struct texture *texture);

void texture_use(const struct texture *texture);

#endif

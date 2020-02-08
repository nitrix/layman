#ifndef LEARNGL_TEXTURE_H
#define LEARNGL_TEXTURE_H

#include "shader.h"

struct texture;

struct texture *texture_load(const char *filepath);
void texture_destroy(struct texture *texture);

void texture_use(const struct texture *texture);

#endif

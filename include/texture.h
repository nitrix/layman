#ifndef LEARNGL_TEXTURE_H
#define LEARNGL_TEXTURE_H

struct texture;

struct texture *texture_load(const char *filepath);
void texture_destroy(struct texture *texture);

void texture_use(struct texture *texture);
void texture_unuse(struct texture *texture);

#endif

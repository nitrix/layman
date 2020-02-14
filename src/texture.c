#include "texture.h"

void texture_use(const struct texture *texture) {
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

void texture_destroy(struct texture *texture) {
    glDeleteTextures(1, &texture->texture_id);
}

#include "toolkit.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct texture {
    GLuint texture_id;
};

void texture_use(const struct texture *texture) {
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

struct texture *texture_load(const char *filepath) {
    struct texture *texture = malloc(sizeof *texture);

    if (!texture) {
        return NULL;
    }

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    int width, height, comp;
    unsigned char *image = stbi_load(filepath, &width, &height, &comp, STBI_rgb);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);

    // Clamping pixels that are out of bound.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Filtering (what happens when the texture's pixels don't quite match the resolution of what's being pasted on).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

void texture_destroy(struct texture *texture) {
    glDeleteTextures(1, &texture->texture_id);
}
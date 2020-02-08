#include "toolkit.h"

#include <GL/glew.h>
#include <SOIL/SOIL.h>

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

    int width, height;
    unsigned char *image = SOIL_load_image(filepath, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

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
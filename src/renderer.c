#include "renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

void renderer_init(void) {
    // Ask flew to use a modern approach to obtain the OpenGL function pointers.
    glewExperimental = GL_TRUE;

    glewInit();
}

void renderer_fini(void) {
    // TODO
}

void renderer_match_viewport(struct renderer *renderer, struct window *window) {
    int width, height;
    window_framebuffer_size(window, &width, &height);
    renderer_set_viewport(renderer, width, height);
}

void renderer_set_viewport(struct renderer *renderer, int width, int height) {
    glViewport(0, 0, width, height);
}

struct renderer *renderer_create(void) {
    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        return NULL;
    }

    renderer->color = 0.2f;

    return renderer;
}

void renderer_clear(struct renderer *renderer) {
    renderer->color += 0.01f;
    if (renderer->color >= 1.0f) {
        renderer->color = 0.2f;
    }

    glClearColor(renderer->color, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_render(struct renderer *renderer) {
    // TODO: Draw stuff
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
}
#include <GL/glew.h>

#include "renderer.h"
#include "toolkit.h"

struct renderer {
    float color;
};

atomic_uint renderer_count;

void renderer_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&renderer_count, 1);

    if (previous_count == 0) {
        // Ask glew to use a more modern approach for obtaining the OpenGL function pointers.
        glewExperimental = GL_TRUE;

        glewInit();
    }
}

void renderer_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&renderer_count, 1);

    if (previous_count == 1) {
        // TODO
    }
}

void renderer_match_viewport(struct renderer *renderer, struct window *window) {
    int width, height;
    window_framebuffer_size(window, &width, &height);
    renderer_set_viewport(renderer, width, height);
}

void renderer_set_viewport(struct renderer *renderer, int width, int height) {
    TK_UNUSED(renderer);
    glViewport(0, 0, width, height);
}

struct renderer *renderer_create(void) {
    renderer_increment();

    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        renderer_decrement();
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
    TK_UNUSED(renderer);
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
    renderer_decrement();
}
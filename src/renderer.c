#include "model.h"
#include "toolkit.h"
#include "window.h"
#include "shader.h"
#include <GL/glew.h>

struct renderer {
    struct window *window;
};

atomic_uint renderer_count;

void renderer_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&renderer_count, 1);

    if (previous_count == 0) {
        // Ask glew to use a more modern approach for obtaining the OpenGL function pointers.
        glewExperimental = GL_TRUE;

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            fprintf(stderr, "glewInit failed: %s\n", glewGetErrorString(err));
            exit(EXIT_FAILURE);
        }
    }
}

void renderer_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&renderer_count, 1);

    if (previous_count == 1) {
        // TODO: Do we not have to de-initialize glew when we're done?
    }
}

void renderer_set_viewport(struct renderer *renderer, int width, int height) {
    TK_UNUSED(renderer);
    glViewport(0, 0, width, height);
}

void renderer_match_viewport(struct renderer *renderer, struct window *window) {
    int width, height;
    window_framebuffer_size(window, &width, &height);
    renderer_set_viewport(renderer, width, height);
}

struct renderer *renderer_create(struct window *window) {
    window_switch_context(window);
    renderer_increment();

    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        renderer_decrement();
        return NULL;
    }

    renderer_match_viewport(renderer, window);

    renderer->window = window;

    return renderer;
}

void renderer_clear(struct renderer *renderer) {
    TK_UNUSED(renderer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_render(struct renderer *renderer, struct model *model, struct shader *shader) {
    TK_UNUSED(renderer);

    model_use(model);
    shader_use(shader);

    // This is for debugging
    // shader_validate(shader);

    glDrawElements(GL_TRIANGLES, 3 * model_vertex_count(model), GL_UNSIGNED_INT, 0);
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
    renderer_decrement();
}

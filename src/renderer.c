#include "model.h"
#include "toolkit.h"
#include "window.h"

#include <GL/glew.h>

struct renderer {
    int dummy;
};

atomic_uint renderer_count;

void renderer_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&renderer_count, 1);

    if (previous_count == 0) {
        // Ask glew to use a more modern approach for obtaining the OpenGL function pointers.
        glewExperimental = GL_TRUE;

        // glutInitDisplayMode(GLUT_RGB);
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
        // TODO
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

struct renderer *renderer_create(void) {
    renderer_increment();

    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        renderer_decrement();
        return NULL;
    }

    return renderer;
}

void renderer_clear(struct renderer *renderer) {
    TK_UNUSED(renderer);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_render(struct renderer *renderer, struct model *model) {
    // Wire frame
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Back to normal
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    model_use(model);

    // glDrawArrays(GL_TRIANGLES, 0, model_vertex_count(model));
    glDrawElements(GL_TRIANGLES, model_index_count(model), GL_UNSIGNED_INT, 0);

    model_unuse(model);

    // TODO: Draw stuff
    TK_UNUSED(renderer);
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
    renderer_decrement();
}

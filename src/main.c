#include "main.h"

float vertices[] = {
    -0.5f,  0.5f, 0.0f, // V0
    -0.5f, -0.5f, 0.0f, // V1
     0.5f, -0.5f, 0.0f, // V2
     0.5f,  0.5f, 0.0f, // V3
};

unsigned int indices[] = {
    0, 1, 3, // Top left triangle (V0, V1, V3)
    3, 1, 2, // Bottom right triangle (V3, V1, V2)
};

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");

    window_switch_context(window); // TODO: Maybe the renderer should take a window, or both be merged even.
    struct renderer *renderer = renderer_create();

    window_switch_context(window);
    renderer_match_viewport(renderer, window);

    main_loop(window, renderer);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    struct model *example_model = model_create_raw(vertices, TK_COUNT(vertices), indices, TK_COUNT(indices));

    while (!window_should_close(window)) {
        window_handle_events(window);
        renderer_clear(renderer);
        renderer_render(renderer, example_model);
        window_refresh(window);
    }

    model_destroy(example_model);
}

#include "main.h"

float vertices[] = {
    -0.5f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
};

/* This is a very long comment that spawns multiple lines because it's just way too long to stay on the same one. I
 * wonder how clang will deal with this, especially when it's utter garbage like right now. */
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
    struct model *example_model = model_create_raw(vertices, TK_COUNT(vertices));

    while (!window_should_close(window)) {
	window_handle_events(window);
	renderer_clear(renderer);
	renderer_render(renderer, example_model);
	window_refresh(window);
    }

    model_destroy(example_model);
}

#include <GLFW/glfw3.h>
#include "main.h"

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");
    struct renderer *renderer = renderer_create();

    window_switch_context(window);
    renderer_match_viewport(renderer, window);

    main_loop(window, renderer);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    while (!window_should_close(window)) {
        glfwPollEvents(); // TODO: Temporary way of handling events.

        renderer_clear(renderer);
        renderer_render(renderer);
        window_refresh(window);
    }
}

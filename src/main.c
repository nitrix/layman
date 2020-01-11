#include "main.h"
#include "shader.h"

float vertices[] = {
    -0.5f,  0.5f, 0.0f, // V0
    -0.5f, -0.5f, 0.0f, // V1
     0.5f, -0.5f, 0.0f, // V2
     0.5f,  0.5f, 0.0f, // V3
};

unsigned int faces[] = {
    0, 1, 3, // Top left triangle (V0, V1, V3)
    3, 1, 2, // Bottom right triangle (V3, V1, V2)
};

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");
    struct renderer *renderer = renderer_create(window);

    main_loop(window, renderer);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    // Prepare example model and shader
    struct model *example_model = model_create_from_raw(vertices, TK_COUNT(vertices) / 3, faces, TK_COUNT(faces) / 3);
    struct shader *example_shader = shader_load_by_name("example");

    // Bind model buffers to shader input variables by name
    shader_bind(example_shader, MODEL_BUFFER_VERTICES, "position");

    while (!window_should_close(window)) {
        window_handle_events(window);

        renderer_clear(renderer);
        renderer_render(renderer, example_model, example_shader);

        window_refresh(window);
    }

    shader_destroy(example_shader);
    model_destroy(example_model);
}

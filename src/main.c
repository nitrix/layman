#include "main.h"

#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"

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

float texture_uvs[] = {
    0.0f, 0.0f, // V0
    0.0f, 1.0f, // V1
    1.0f, 1.0f, // V2
    1.0f, 0.0f, // V3
};

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");
    struct renderer *renderer = renderer_create(window, 0.610865f, 0.1f, 1000); // 70 FOV

    main_loop(window, renderer);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    // Prepare example model, shader and texture
    struct model *example_model = model_create_from_raw(vertices, TK_COUNT(vertices) / 3, faces, TK_COUNT(faces) / 3, texture_uvs);
    struct shader *example_shader = shader_load_by_name("example");
    struct texture *example_texture = texture_load("textures/example.png");

    // Prepare example entity
    struct entity *example_entity = entity_create();
    example_entity->position = (struct vector3f) { 0.0f, 0.0f, -1.0f };
    example_entity->model = example_model;
    example_entity->shader = example_shader;
    example_entity->texture = example_texture;

    while (!window_should_close(window)) {
        // TODO: Elapsed time frame, needs more investigation.

        // entity_rotate(example_entity, 0.010f, 0.0f, 0.0f);
        // entity_move(example_entity, 0.01f, 0.0f, 0.0f);
        // entity_move(example_entity, 0.0f, 0.0f, -0.01f);

        window_handle_events(window);

        renderer_clear(renderer);
        renderer_render(renderer, example_entity);

        window_refresh(window);
    }

    texture_destroy(example_texture);
    shader_destroy(example_shader);
    model_destroy(example_model);
}

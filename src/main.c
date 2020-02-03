#include "main.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"
#include "obj.h"

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");
    struct renderer *renderer = renderer_create(window);

    window_use(window);
    renderer_use(renderer);

    main_loop(window, renderer);

    renderer_unuse(renderer);
    window_unuse(window);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    // Prepare camera
    struct camera *camera = camera_create();
    // camera_move(camera, 0.0f, 0.0f, 30.0f);
    camera_move(camera, 0.0f, 0.0f, 15.0f);

    // Prepare light
    struct light *light = light_create();
    light_move(light, 0.0f, 0.0f, 50.0f);

    // Prepare example model, shader and texture
    struct model *example_model = obj_load_model("models/wakfu.obj");
    struct shader *example_shader = shader_load_by_name("wakfu");
    struct texture *example_texture = texture_load("textures/wakfu.png");

    struct model *example_bunny_model = obj_load_model("models/bunny.obj");
    struct texture *example_bunny_texture = texture_load("textures/bunny.png");

    // Prepare example entity
    struct entity *example_entity = entity_create();
    example_entity->model = example_model;
    example_entity->shader = example_shader;
    example_entity->texture = example_texture;
    example_entity->shine_damper = 50;
    example_entity->reflectivity = 0.5f;
    entity_rotate(example_entity, 0, -3.0f, 0);
    entity_set_position(example_entity, 0.0f, 0.0f, 0.0f);

    // Prepare example player
    struct entity *example_player = entity_create();
    example_player->model = example_bunny_model;
    example_player->shader = example_shader;
    example_player->texture = example_bunny_texture;
    example_player->shine_damper = 50;
    example_player->reflectivity = 0.0f;
    entity_rotate(example_player, 0, -3.0f, 0);
    entity_set_position(example_player, 5.0f, -1.0f, 5.0f);

    while (!window_should_close(window)) {
        window_handle_events(window, renderer, example_player, camera);

        renderer_clear(renderer);

        camera_relative_to_entity(camera, example_player);
        renderer_render(renderer, camera, light, example_entity);
        renderer_render(renderer, camera, light, example_player);

        window_refresh(window);
    }

    model_destroy(example_bunny_model);
    texture_destroy(example_bunny_texture);

    entity_destroy(example_entity);
    texture_destroy(example_texture);
    shader_destroy(example_shader);
    model_destroy(example_model);
    light_destroy(light);
    camera_destroy(camera);
}

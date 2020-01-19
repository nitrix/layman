#include "main.h"

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"
#include "obj.h"

#include <GLFW/glfw3.h>

int main(int argc, char *argv[]) {
    struct window *window = window_create(1280, 720, "Learn OpenGL");
    struct renderer *renderer = renderer_create(window, 0.610865f, 0.1f, 1000); // 70 FOV

    main_loop(window, renderer);

    renderer_destroy(renderer);
    window_destroy(window);

    return EXIT_SUCCESS;
}

void main_loop(struct window *window, struct renderer *renderer) {
    // Camera
    struct camera *camera = camera_create();

    // Light
    struct light *light = light_create();

    // Prepare example model, shader and texture
    struct model *example_model = obj_load_model("models/wakfu.obj");
    struct shader *example_shader = shader_load_by_name("wakfu");
    struct texture *example_texture = texture_load("textures/wakfu.png");

    // Prepare example entity
    struct entity *example_entity = entity_create();
    example_entity->position = (struct vector3f) { 0.0f, 0.0f, 0.0f };
    example_entity->model = example_model;
    example_entity->shader = example_shader;
    example_entity->texture = example_texture;

    // Zoom-out the camera from the origin
    camera_move(camera, 0.0f, 0.0f, 8.0f);

    // Move the light way back
    light_move(light, 0.0f, 0.0f, 15.0f);

    double last_time = glfwGetTime();
    size_t nb_frames = 0;

    while (!window_should_close(window)) {
        double current_time = glfwGetTime();
        nb_frames++;
        if (current_time - last_time >= 1.0) {
            printf("%f ms/frame\n", 1000.0 / nb_frames);
            nb_frames = 0;
            last_time += 1.0;
        }

        // TODO: Use elapsed time frame time for smoother animations?

        // TODO: Temporary, the rotation currently is around the world origin instead of model origin, which is wrong.
        // entity_rotate(example_entity, 0.01f, 0.02f, 0.03f);
        entity_rotate(example_entity, 0, 0.01f, 0);
        // entity_move(example_entity, 0.01f, 0.0f, 0.0f);
        // entity_move(example_entity, 0.0f, 0.0f, -0.01f);

        window_handle_events(window, renderer, camera);

        renderer_clear(renderer);
        renderer_render(renderer, camera, light, example_entity);

        window_refresh(window);
    }

    entity_destroy(example_entity);
    texture_destroy(example_texture);
    shader_destroy(example_shader);
    model_destroy(example_model);
    light_destroy(light);
    camera_destroy(camera);
}

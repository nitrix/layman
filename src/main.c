#include "main.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"
#include "obj.h"

#define MOVEMENT_SPEED_PER_SECOND 0.0015
#define ROTATION_SPEED_PER_SECOND 0.0003
#define SCROLL_SPEED 0.5f

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

    /*
        camera_change_pitch(camera, y * 0.002f);
        camera_change_angle_around_pivot(camera, x * 0.002f);
        camera_rotate(camera, 0.01f, 0, 0);
     */

    direction_mask direction = TK_MASK_INITIALIZER;
    bool left_button_active = false;
    bool right_button_active = false;
    int x, y;

    while (!window_should_close(window)) {
        tk_result result;

        float elapsed_seconds = window_elapsed_seconds(window);

        do {
            result = window_poll_event(window);
            if (TK_RESULT_IS_FAILURE(result)) break;

            if (window_event_key_pressed(window, '\033')) {
                window_close(window);
            }
            else if (window_event_key_pressed(window, '1')) {
                renderer_set_wireframe(renderer, false);
            }
            else if (window_event_key_pressed(window, '2')) {
                renderer_set_wireframe(renderer, true);
            }
            else if (window_event_key_pressed(window, 'w')) {
                TK_MASK_SET(direction, DIRECTION_FORWARD);
            }
            else if (window_event_key_pressed(window, 's')) {
                TK_MASK_SET(direction, DIRECTION_BACKWARD);
            }
            else if (window_event_key_pressed(window, 'a')) {
                TK_MASK_SET(direction, DIRECTION_LEFT);
            }
            else if (window_event_key_pressed(window, 'd')) {
                TK_MASK_SET(direction, DIRECTION_RIGHT);
            }
            else if (window_event_key_released(window, 'w')) {
                TK_MASK_UNSET(direction, DIRECTION_FORWARD);
            }
            else if (window_event_key_released(window, 's')) {
                TK_MASK_UNSET(direction, DIRECTION_BACKWARD);
            }
            else if (window_event_key_released(window, 'a')) {
                TK_MASK_UNSET(direction, DIRECTION_LEFT);
            }
            else if (window_event_key_released(window, 'd')) {
                TK_MASK_UNSET(direction, DIRECTION_RIGHT);
            }
            else if (window_event_mouse_wheel(window, &x, &y)) {
                camera_change_zoom(camera, SCROLL_SPEED * y);
            }
            else if (window_event_mouse_button_pressed(window, 1)) {
                left_button_active = true;
            }
            else if (window_event_mouse_button_pressed(window, 3)) {
                right_button_active = true;
            }
            else if (window_event_mouse_button_released(window, 1)) {
                left_button_active = false;
            }
            else if (window_event_mouse_button_released(window, 3)) {
                right_button_active = false;
            }
            else if (left_button_active && window_event_mouse_motion_relative(window, &x, &y)) {
                camera_change_angle_around_pivot(camera, x * 0.02f);
            }
            else if (right_button_active && window_event_mouse_motion_relative(window, &x, &y)) {
                camera_change_pitch(camera, y * 0.02f);
            }
        } while (TK_RESULT_IS_SUCCESS(result));

        entity_relative_move(example_player, direction, elapsed_seconds * MOVEMENT_SPEED_PER_SECOND, elapsed_seconds * ROTATION_SPEED_PER_SECOND);
        camera_relative_to_entity(camera, example_player);

        renderer_clear(renderer);
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

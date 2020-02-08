#include "main.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"
#include "obj.h"

#define MOVEMENT_SPEED_PER_SECOND 7.0f
#define ROTATION_SPEED_PER_SECOND 2.0f
#define SCROLL_SPEED 0.5f
#define DRAG_SPEED 0.05f

// TODO: Clang format

int main(int argc, char *argv[]) {
    struct game_state state;

    state.window = window_create(1280, 720, "Learn OpenGL");
    state.renderer = renderer_create(state.window);

    window_use(state.window);
    renderer_use(state.renderer);

    before_loop(&state);
    main_loop(&state);
    after_loop(&state);

    renderer_unuse(state.renderer);
    window_unuse(state.window);

    renderer_destroy(state.renderer);
    window_destroy(state.window);

    return EXIT_SUCCESS;
}

void before_loop(struct game_state *state) {
    // Prepare camera
    state->camera = camera_create();
    camera_move(state->camera, 0.0f, 0.0f, 15.0f);

    // Prepare light
    state->light = light_create();
    light_move(state->light, 0.0f, 0.0f, 50.0f);

    // Prepare player direction
    state->player_direction = TK_MASK_INITIALIZER;

    // Prepare example model, shader and texture
    state->model_shader = shader_load_by_name("model");

    // Prepare example wakfu entity
    state->example_wakfu_entity = entity_create();
    state->example_wakfu_entity->model = obj_load_model("models/wakfu.obj");
    state->example_wakfu_entity->texture = texture_load("textures/wakfu.png");
    state->example_wakfu_entity->shader = state->model_shader;
    state->example_wakfu_entity->shine_damper = 50;
    state->example_wakfu_entity->reflectivity = 0.5f;
    entity_rotate(state->example_wakfu_entity, 0, -3.0f, 0);
    entity_set_position(state->example_wakfu_entity, 0.0f, 0.0f, 0.0f);

    // Prepare example bunny entity
    state->example_bunny_entity = entity_create();
    state->example_bunny_entity->shader = state->model_shader;
    state->example_bunny_entity->model = obj_load_model("models/bunny.obj");
    state->example_bunny_entity->texture = texture_load("textures/bunny.png");
    state->example_bunny_entity->shine_damper = 50;
    state->example_bunny_entity->reflectivity = 0.0f;
    entity_rotate(state->example_bunny_entity, 0, -3.0f, 0);
    entity_set_position(state->example_bunny_entity, 5.0f, -1.0f, 5.0f);
}

void after_loop(struct game_state *state) {
    // Cleanups
    light_destroy(state->light);
    camera_destroy(state->camera);
    shader_destroy(state->model_shader);

    // Wakfu entity
    model_destroy(state->example_wakfu_entity->model);
    texture_destroy(state->example_wakfu_entity->texture);
    entity_destroy(state->example_wakfu_entity);

    // Bunny entity
    model_destroy(state->example_bunny_entity->model);
    texture_destroy(state->example_bunny_entity->texture);
    entity_destroy(state->example_bunny_entity);
}

void main_loop(struct game_state *state) {
    window_set_custom(state->window, state);
    window_set_key_callback(state->window, on_key_callback);
    window_set_mouse_button_callback(state->window, on_mouse_button_callback);
    window_set_mouse_position_callback(state->window, on_mouse_position_callback);
    window_set_mouse_wheel_callback(state->window, on_mouse_wheel_callback);

    while (!window_should_close(state->window)) {
        window_poll_events(state->window);

        float elapsed_seconds = window_elapsed_seconds(state->window);

        entity_relative_move(state->example_bunny_entity, state->player_direction, elapsed_seconds * MOVEMENT_SPEED_PER_SECOND, elapsed_seconds * ROTATION_SPEED_PER_SECOND);
        camera_relative_to_entity(state->camera, state->example_bunny_entity);

        renderer_clear(state->renderer);

        // TODO: Iterate entities
        // TODO: Should do grouping to avoid a ton of model_use and shader_use inside that renderer_render.
        renderer_render(state->renderer, state->camera, state->light, state->example_wakfu_entity);
        renderer_render(state->renderer, state->camera, state->light, state->example_bunny_entity);

        window_refresh(state->window);
    }
}

void on_key_callback(struct window *window, int key, enum window_action action) {
    struct game_state *state = window_custom(window);

    // Escape key closes the window
    if (key == 256 && action == WINDOW_ACTION_PRESS) {
        window_close(window);
    }

    // Digit 1 and 2 controls the wireframe mode
    else if ((key == 49 || key == 50) && action == WINDOW_ACTION_PRESS) {
        renderer_set_wireframe(state->renderer, key == 50);
    }

    // W,A,S,D
    else if (key == 'W' && action == WINDOW_ACTION_PRESS) {
        TK_MASK_SET(state->player_direction, DIRECTION_FORWARD * 1u);
    }
    else if (key == 'W' && action == WINDOW_ACTION_RELEASE) {
        TK_MASK_UNSET(state->player_direction, DIRECTION_FORWARD * 1u);
    }
    else if (key == 'S' && action == WINDOW_ACTION_PRESS) {
        TK_MASK_SET(state->player_direction, DIRECTION_BACKWARD * 1u);
    }
    else if (key == 'S' && action == WINDOW_ACTION_RELEASE) {
        TK_MASK_UNSET(state->player_direction, DIRECTION_BACKWARD * 1u);
    }
    else if (key == 'A' && action == WINDOW_ACTION_PRESS) {
        TK_MASK_SET(state->player_direction, DIRECTION_LEFT * 1u);
    }
    else if (key == 'A' && action == WINDOW_ACTION_RELEASE) {
        TK_MASK_UNSET(state->player_direction, DIRECTION_LEFT * 1u);
    }
    else if (key == 'D' && action == WINDOW_ACTION_PRESS) {
        TK_MASK_SET(state->player_direction, DIRECTION_RIGHT * 1u);
    }
    else if (key == 'D' && action == WINDOW_ACTION_RELEASE) {
        TK_MASK_UNSET(state->player_direction, DIRECTION_RIGHT * 1u);
    }
}

void on_mouse_position_callback(struct window *window, double x, double y) {
    struct game_state *state = window_custom(window);

    if (state->dragging_horizontally) {
        camera_change_angle_around_pivot(state->camera, (x - state->last_cursor.x) * DRAG_SPEED);
    }

    if (state->dragging_vertically) {
        camera_change_pitch(state->camera, (y - state->last_cursor.y) * DRAG_SPEED);
    }

    state->last_cursor.x = x;
    state->last_cursor.y = y;
}

void on_mouse_button_callback(struct window *window, int button, enum window_action action) {
    struct game_state *state = window_custom(window);

    if (button == 0 && action == WINDOW_ACTION_PRESS) {
        state->dragging_horizontally = true;
    }
    else if (button == 0 && action == WINDOW_ACTION_RELEASE) {
        state->dragging_horizontally = false;
    }
    else if (button == 1 && action == WINDOW_ACTION_PRESS) {
        state->dragging_vertically = true;
    }
    else if (button == 1 && action == WINDOW_ACTION_RELEASE) {
        state->dragging_vertically = false;
    }
}

void on_mouse_wheel_callback(struct window *window, double delta) {
    struct game_state *state = window_custom(window);

    camera_change_zoom(state->camera, delta * SCROLL_SPEED);
}

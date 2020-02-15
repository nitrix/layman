#ifndef LEARNGL_MAIN_H
#define LEARNGL_MAIN_H

#include "renderer.h"
#include "toolkit.h"
#include "window.h"
#include "direction.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "texture.h"
#include "loader.h"
#include "material.h"
#include "terrain.h"

struct game_state {
    struct window *window;
    struct renderer *renderer;

    struct camera *camera;
    struct light *light;
    struct material *material;
    struct shader *model_shader;
    struct shader *terrain_shader;

    direction_mask player_direction;

    bool dragging_horizontally;
    bool dragging_vertically;
    struct { double x, y; } last_cursor;

    // TODO: These should not be needed and can entirely be represented by some ID from some kind of scene manager.
    struct entity *example_wakfu_entity;
    struct entity *example_bunny_entity;
    struct entity *example_terrain_entity;
};

int main(int argc, char *argv[]);

void before_loop(struct game_state *state);
void main_loop( struct game_state *state);
void after_loop(struct game_state *state);

void on_key_callback(struct window *window, int key, enum window_action action);
void on_mouse_button_callback(struct window *window, int button, enum window_action action);
void on_mouse_position_callback(struct window *window, double x, double y);
void on_mouse_wheel_callback(struct window *window, double delta);

#endif

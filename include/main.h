#ifndef LEARNGL_MAIN_H
#define LEARNGL_MAIN_H

#include "renderer.h"
#include "toolkit.h"
#include "window.h"
#include "direction.h"

struct game_state {
    struct window *window;
    struct renderer *renderer;

    struct camera *camera;
    struct light *light;
    struct shader *model_shader;

    direction_mask player_direction;

    // TODO: These should not be needed and can entirely be represented by some ID from some kind of scene manager.
    struct entity *example_wakfu_entity;
    struct entity *example_bunny_entity;
};

int main(int argc, char *argv[]);

void before_loop(struct game_state *state);
void main_loop( struct game_state *state);
void after_loop(struct game_state *state);

void on_key_func(struct window *window, int key, enum window_key_action action);

#endif

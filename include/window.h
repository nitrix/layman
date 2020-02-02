#ifndef LEARNGL_WINDOW_H
#define LEARNGL_WINDOW_H

#include "camera.h"
#include "toolkit.h"

struct window;
struct renderer;
struct entity;

// TODO: const everywhere
// TODO: Do all TODOs

struct window *window_create(int width, int height, const char *title);
void window_destroy(struct window *window);

void window_use(struct window *window);
void window_unuse(struct window *window);

double window_current_time(struct window *window);
void window_handle_events(struct window *window, struct renderer *renderer, struct entity *player, struct camera *camera);
void window_framebuffer_size(struct window *window, int *width, int *height);
void window_refresh(struct window *window);
bool window_should_close(struct window *window);

#endif
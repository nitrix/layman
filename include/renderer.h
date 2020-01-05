#ifndef LEARNGL_RENDERER_H
#define LEARNGL_RENDERER_H

#include <stdbool.h>
#include "window.h"

struct renderer {
    float color;
};

void renderer_init(void);
void renderer_fini(void);

struct renderer *renderer_create(void);
void renderer_destroy(struct renderer *renderer);

void renderer_set_viewport(struct renderer *renderer, int width, int height);
void renderer_match_viewport(struct renderer *renderer, struct window *window);

void renderer_clear(struct renderer *renderer);
void renderer_render(struct renderer *renderer);

#endif
#ifndef LEARNGL_RENDERER_H
#define LEARNGL_RENDERER_H

#include "model.h"
#include "window.h"

struct renderer;

struct renderer *renderer_create(void);
void renderer_destroy(struct renderer *renderer);

void renderer_set_viewport(struct renderer *renderer, int width, int height);
void renderer_match_viewport(struct renderer *renderer, struct window *window);

void renderer_clear(struct renderer *renderer);
void renderer_render(struct renderer *renderer, struct model *model);

#endif
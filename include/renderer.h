#ifndef LEARNGL_RENDERER_H
#define LEARNGL_RENDERER_H

#include "camera.h"
#include "window.h"
#include "light.h"

struct renderer;
struct entity;

struct renderer *renderer_create(struct window *window);
void renderer_destroy(struct renderer *renderer);

void renderer_use(struct renderer *renderer);
void renderer_unuse(struct renderer *renderer);

void renderer_set_viewport(struct renderer *renderer, int width, int height);
void renderer_match_viewport(struct renderer *renderer, struct window *window);
void renderer_set_wireframe(struct renderer *renderer, bool b);

void renderer_clear(struct renderer *renderer);
void renderer_render(struct renderer *renderer, struct camera *camera, struct light *light, struct entity *entity);

const struct matrix4f *renderer_projection_matrix(const struct renderer *renderer);
void renderer_update_projection_matrix(struct renderer *renderer);

#endif

#ifndef LAYMAN_RENDERER_H
#define LAYMAN_RENDERER_H

#include "scene.h"

struct layman_renderer *layman_renderer_create(void);
void layman_renderer_destroy(struct layman_renderer *renderer);

void layman_renderer_clear(struct layman_renderer *renderer);
void layman_renderer_render(struct layman_renderer *renderer, const struct layman_scene *scene);

#endif
#ifndef LAYMAN_RENDERER_H
#define LAYMAN_RENDERER_H

#include "camera.h"
#include "scene.h"

// TODO: Documentation.
struct layman_renderer *layman_renderer_create(void);

// TODO: Documentation.
void layman_renderer_destroy(struct layman_renderer *renderer);

// TODO: Documentation.
void layman_renderer_use(struct layman_renderer *renderer);

// TODO: Documentation.
void layman_renderer_unuse(struct layman_renderer *renderer);

// TODO: Documentation.
void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene);

#endif

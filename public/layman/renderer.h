#ifndef LAYMAN_RENDERER_H
#define LAYMAN_RENDERER_H

#include "camera.h"
#include "environment.h"
#include "scene.h"

// TODO: Documentation.
struct layman_renderer *layman_renderer_create(void);

// TODO: Documentation.
void layman_renderer_destroy(struct layman_renderer *renderer);

// TODO: Documentation.
void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene, const struct layman_environment *environment);

// TODO: Documentation.
double layman_renderer_elapsed(const struct layman_renderer *renderer);

#endif

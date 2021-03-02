#ifndef LAYMAN_PUBLIC_RENDERER_H
#define LAYMAN_PUBLIC_RENDERER_H

#include "camera.h"
#include "scene.h"
#include "window.h"

// TODO: Documentation.
struct layman_renderer *layman_renderer_create(const struct layman_window *window);
void layman_renderer_destroy(struct layman_renderer *renderer);
void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene);
void layman_renderer_wireframe(struct layman_renderer *renderer, bool enabled);

#endif

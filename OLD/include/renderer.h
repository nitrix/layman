#ifndef LEARNGL_RENDERER_H
#define LEARNGL_RENDERER_H

#include "camera.h"
#include "window.h"
#include "entity.h"
#include "light.h"
#include "entity.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "toolkit.h"
#include "window.h"
#include "math.h"
#include "terrain.h"

#include <glad/glad.h>

struct renderer {
    struct window *window;

    size_t viewport_width;
    size_t viewport_height;

    float fov;
    float near_plane;
    float far_plane;

    struct matrix4f projection_matrix;
};

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

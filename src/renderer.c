#include "entity.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "toolkit.h"
#include "window.h"
#include "math.h"
#include "renderer.h"

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

atomic_uint renderer_count;

void renderer_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&renderer_count, 1);

    if (previous_count == 0) {
        if (!gladLoadGL()) {
            fprintf(stderr, "gladLoadGL() failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

void renderer_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&renderer_count, 1);
    if (previous_count == 1) {
        TK_UNUSED(previous_count);
    }
}

void renderer_set_viewport(struct renderer *renderer, int width, int height) {
    renderer->viewport_width = width;
    renderer->viewport_height = height;
    glViewport(0, 0, width, height);
}

void renderer_match_viewport(struct renderer *renderer, struct window *window) {
    int width, height;
    window_framebuffer_size(window, &width, &height);
    renderer_set_viewport(renderer, width, height);
}

struct renderer *renderer_create(struct window *window) {
    window_use(window);
    renderer_increment();

    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        renderer_decrement();
        return NULL;
    }

    renderer_match_viewport(renderer, window);

    renderer->window = window;
    renderer->fov = 0.610865f;
    renderer->near_plane = 0.1f;
    renderer->far_plane = 1000.0f;

    renderer_update_projection_matrix(renderer);

    return renderer;
}

void renderer_clear(struct renderer *renderer) {
    TK_UNUSED(renderer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    unsigned int mask = 0;
    mask |= (unsigned int) GL_COLOR_BUFFER_BIT;
    mask |= (unsigned int) GL_DEPTH_BUFFER_BIT;
    glClear(mask);
}

void renderer_use(struct renderer *renderer) {
    glEnable(GL_DEPTH_TEST);

    // Back face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void renderer_unuse(struct renderer *renderer) {
    TK_UNUSED(renderer);
}

void renderer_set_wireframe(struct renderer *renderer, bool b) {
    TK_UNUSED(renderer);

    glPolygonMode(GL_FRONT_AND_BACK, b ? GL_LINE : GL_FILL);
}

void renderer_render(struct renderer *renderer, struct camera *camera, struct light *light, struct entity *entity) {
    model_use(entity->model);
    shader_use(entity->shader);
    texture_use(entity->texture);

    // Uniforms
    const struct matrix4f *model = entity_model_matrix(entity);
    shader_bind_uniform_model(entity->shader, model);

    const struct matrix4f *view = camera_view_matrix(camera);
    shader_bind_uniform_view(entity->shader, view);

    const struct matrix4f *projection = renderer_projection_matrix(renderer);
    shader_bind_uniform_projection(entity->shader, projection);

    shader_bind_uniform_light(entity->shader, light);
    shader_bind_uniform_entity(entity->shader, entity);

    if (tk_debug_is_enabled()) {
        shader_validate(entity->shader);
    }

    glDrawElements(GL_TRIANGLES, 3 * model_face_count(entity->model), GL_UNSIGNED_INT, 0);
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
    renderer_decrement();
}

const struct matrix4f *renderer_projection_matrix(const struct renderer *renderer) {
    return &renderer->projection_matrix;
}

void renderer_update_projection_matrix(struct renderer *renderer) {
    float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
    float scale_y = (1.0f / tanf(renderer->fov)) * aspect_ratio;
    float scale_x = scale_y / aspect_ratio;
    float frustrum_length = renderer->far_plane - renderer->near_plane;

    renderer->projection_matrix = (struct matrix4f) {
        .x1 = scale_x,
        .y2 = scale_y,
        .z3 = -((renderer->far_plane + renderer->near_plane) / frustrum_length),
        .z4 = -1,
        .w3 = -((2 * renderer->near_plane * renderer->far_plane) / frustrum_length)
    };
}

#include "entity.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "toolkit.h"
#include "window.h"
#include "math.h"
#include "renderer.h"

#include <GL/glew.h>

struct renderer {
    struct window *window;

    size_t viewport_width;
    size_t viewport_height;

    float fov;
    float near_plane;
    float far_plane;
};

atomic_uint renderer_count;

void renderer_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&renderer_count, 1);

    if (previous_count == 0) {
        // Ask glew to use a more modern approach for obtaining the OpenGL function pointers.
        glewExperimental = GL_TRUE;

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            fprintf(stderr, "glewInit failed: %s\n", glewGetErrorString(err));
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

struct renderer *renderer_create(struct window *window, float fov, float near_plane, float far_plane) {
    window_switch_context(window);
    renderer_increment();

    struct renderer *renderer = malloc(sizeof *renderer);

    if (!renderer) {
        renderer_decrement();
        return NULL;
    }

    renderer_match_viewport(renderer, window);

    renderer->window = window;
    renderer->fov = fov;
    renderer->near_plane = near_plane;
    renderer->far_plane = far_plane;

    glEnable(GL_DEPTH_TEST);

    return renderer;
}

void renderer_clear(struct renderer *renderer) {
    TK_UNUSED(renderer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glClear(GL_DEPTH_BUFFER_BIT);
}

void renderer_render(struct renderer *renderer, struct camera *camera, struct entity *entity) {
    TK_UNUSED(renderer);

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    // glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    model_use(entity->model);
    shader_use(entity->shader);
    texture_use(entity->texture);

    struct matrix4f transformation = matrix_create_from_transformation(entity->position, entity->rotation, entity->scale);
    shader_bind_uniform_matrix4f(entity->shader, "transformation", transformation);

    struct matrix4f projection = renderer_projection_matrix(renderer);
    shader_bind_uniform_matrix4f(entity->shader, "projection", projection);

    struct matrix4f view = camera_view_matrix(camera);
    shader_bind_uniform_matrix4f(entity->shader, "view", view);

    // This is for debugging
    // shader_validate(shader);

    glDrawElements(GL_TRIANGLES, 3 * model_face_count(entity->model), GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, model_vertex_count(entity->model));
}

void renderer_destroy(struct renderer *renderer) {
    free(renderer);
    renderer_decrement();
}

struct matrix4f renderer_projection_matrix(struct renderer *renderer) {
    float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
    float scale_y = (1.0f / tanf(renderer->fov)) * aspect_ratio;
    float scale_x = scale_y / aspect_ratio;
    float frustrum_length = renderer->far_plane - renderer->near_plane;

    struct matrix4f m = {
        .x1 = scale_x,
        .y2 = scale_y,
        .z3 = -((renderer->far_plane + renderer->near_plane) / frustrum_length),
        .z4 = -1,
        .w3 = -((2 * renderer->near_plane * renderer->far_plane) / frustrum_length)
    };

    return m;
}
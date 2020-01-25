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

void renderer_set_wireframe(struct renderer *renderer, bool flag) {
    TK_UNUSED(renderer);

    if (flag) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    } else {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

    // glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
}

void renderer_render(struct renderer *renderer, struct camera *camera, struct light *light, struct entity *entity) {
    model_use(entity->model);
    shader_use(entity->shader);
    texture_use(entity->texture);

    // Uniforms
    struct matrix4f projection = renderer_projection_matrix(renderer);
    struct matrix4f view = camera_view_matrix(camera);
    shader_bind_uniforms(entity->shader, &projection, &view);
    shader_bind_uniforms_light(entity->shader, light);
    shader_bind_uniforms_entity(entity->shader, entity);

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
    float near_plane_distance = renderer->far_plane - renderer->near_plane;
    float far_plane_distance = renderer->far_plane + renderer->near_plane;

    struct matrix4f m = {
        .x1 = (1.0f / tanf(renderer->fov)) / aspect_ratio,
        .y2 = (1.0f / tanf(renderer->fov)),
        .z3 = (far_plane_distance * -1.0f) / near_plane_distance,
        .z4 = -1.0f,
        .w3 = (2 * renderer->near_plane * renderer->far_plane * -1.0f) / far_plane_distance,
    };

    return m;
}
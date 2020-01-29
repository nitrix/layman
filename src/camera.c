#include "toolkit.h"
#include "matrix.h"
#include "vector.h"

struct camera {
    struct vector3f position;
    struct vector3f rotation;
    struct matrix4f view_matrix;
};

struct camera *camera_create(void) {
    struct camera *camera = malloc(sizeof *camera);

    if (!camera) {
        return NULL;
    }

    camera->position.x = 0;
    camera->position.y = 0;
    camera->position.z = 0;

    camera->rotation.x = 0;
    camera->rotation.y = 0;
    camera->rotation.z = 0;

    return camera;
}

void camera_destroy(struct camera *camera) {
    free(camera);
}

void camera_update_view_matrix(struct camera *camera) {
    camera->view_matrix = matrix_identity();

    matrix_rotate_x(&camera->view_matrix, camera->rotation.x);
    matrix_rotate_y(&camera->view_matrix, camera->rotation.y);
    matrix_rotate_z(&camera->view_matrix, camera->rotation.z);

    struct vector3f inverse_position = {
        .x = -camera->position.x,
        .y = -camera->position.y,
        .z = -camera->position.z,
    };

    matrix_translate(&camera->view_matrix, &inverse_position);
}

void camera_move(struct camera *camera, float dx, float dy, float dz) {
    camera->position.x += dx;
    camera->position.y += dy;
    camera->position.z += dz;

    camera_update_view_matrix(camera);
}

void camera_rotate(struct camera *camera, float dx, float dy, float dz) {
    camera->rotation.x += dx;
    camera->rotation.y += dy;
    camera->rotation.z += dz;

    camera_update_view_matrix(camera);
}

struct matrix4f *camera_view_matrix(struct camera *camera) {
    return &camera->view_matrix;
}

#include "camera.h"

void camera_update_view_matrix(struct camera *camera);

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

    // 3rd person camera.
    camera->distance_from_pivot = 10;
    camera->pitch = 0;
    camera->angle_around_pivot = 0;

    camera_update_view_matrix(camera);

    return camera;
}

void camera_destroy(struct camera *camera) {
    free(camera);
}

void camera_relative_to_pivot(struct camera *camera, const struct vector3f *pivot_position, const struct vector3f *pivot_rotation) {
    float vertical_distance = camera->distance_from_pivot * sinf(camera->pitch);
    float horizontal_distance = camera->distance_from_pivot * cosf(camera->pitch);
    float theta = pivot_rotation->y + camera->angle_around_pivot;
    float offset_x = horizontal_distance * sinf(theta);
    float offset_z = horizontal_distance * cosf(theta);

    camera->position.y = pivot_position->y + vertical_distance;
    camera->position.x = pivot_position->x + offset_x;
    camera->position.z = pivot_position->z - offset_z;

    camera->rotation.y = M_PI - theta;
    camera->rotation.x = -camera->pitch;

    camera->view_matrix_is_up_to_date = false;
}

void camera_move(struct camera *camera, float dx, float dy, float dz) {
    camera->position.x += dx;
    camera->position.y += dy;
    camera->position.z += dz;

    camera->view_matrix_is_up_to_date = false;
}

void camera_rotate(struct camera *camera, float dx, float dy, float dz) {
    camera->rotation.x += dx;
    camera->rotation.y += dy;
    camera->rotation.z += dz;

    camera->view_matrix_is_up_to_date = false;
}

void camera_change_zoom(struct camera *camera, float zoom) {
    camera->distance_from_pivot -= zoom;

    camera->view_matrix_is_up_to_date = false;
}

void camera_change_pitch(struct camera *camera, float delta) {
    camera->pitch += delta;

    camera->view_matrix_is_up_to_date = false;
}

void camera_change_angle_around_pivot(struct camera *camera, float delta) {
    camera->angle_around_pivot += delta;

    camera->view_matrix_is_up_to_date = false;
}

const struct matrix4f *camera_view_matrix(struct camera *camera) {
    if (!camera->view_matrix_is_up_to_date) {
        camera_update_view_matrix(camera);
    }

    return &camera->view_matrix;
}

void camera_update_view_matrix(struct camera *camera) {
    camera->view_matrix = matrix_identity();

    struct vector3f inverse_position = {
        .x = -camera->position.x,
        .y = -camera->position.y,
        .z = -camera->position.z,
    };

    matrix_translate(&camera->view_matrix, &inverse_position);

    matrix_rotate_y(&camera->view_matrix, camera->rotation.y);
    matrix_rotate_x(&camera->view_matrix, camera->rotation.x);

    camera->view_matrix_is_up_to_date = true;
}

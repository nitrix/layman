#include "camera.h"
#include "math/vector.h"
#include "toolkit.h"
#include "math/matrix.h"

struct camera {
    struct vector3f position;
    float pitch;
    float yaw;
    float roll;
};

struct camera *camera_create(void) {
    struct camera *camera = malloc(sizeof *camera);

    if (!camera) {
        return NULL;
    }

    camera->position.x = 0;
    camera->position.y = 0;
    camera->position.z = 0;

    camera->pitch = 0;
    camera->yaw = 0;
    camera->roll = 0;

    return camera;
}

void camera_destroy(struct camera *camera) {
    free(camera);
}

void camera_move(struct camera *camera, float dx, float dy, float dz) {
    camera->position.x += dx;
    camera->position.y += dy;
    camera->position.z += dz;
}

struct matrix4f camera_view_matrix(struct camera *camera) {
    struct matrix4f m = matrix_identity();

    matrix_rotate_x(&m, camera->pitch);
    matrix_rotate_y(&m, camera->yaw);

    matrix_translate(&m, (struct vector3f) {
        .x = -camera->position.x,
        .y = -camera->position.y,
        .z = -camera->position.z,
    });

    return m;
}
#ifndef LEARNGL_CAMERA_H
#define LEARNGL_CAMERA_H

#include "entity.h"
#include "toolkit.h"
#include "matrix.h"
#include "vector.h"
#include "math.h"

struct camera {
    struct vector3f position;
    struct vector3f rotation;
    struct matrix4f view_matrix;
    bool view_matrix_is_up_to_date;

    float distance_from_pivot;
    float angle_around_pivot;
    float pitch;
};

struct camera *camera_create(void);
void camera_destroy(struct camera *camera);

void camera_move(struct camera *camera, float dx, float dy, float dz);
void camera_rotate(struct camera *camera, float dx, float dy, float dz);

void camera_change_pitch(struct camera *camera, float delta);
void camera_change_angle_around_pivot(struct camera *camera, float delta);
void camera_change_zoom(struct camera *camera, float delta);

void camera_relative_to_pivot(struct camera *camera, const struct vector3f *pivot_position, const struct vector3f *pivot_rotation);

const struct matrix4f *camera_view_matrix(struct camera *camera);

#endif

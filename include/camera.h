#ifndef LEARNGL_CAMERA_H
#define LEARNGL_CAMERA_H

struct camera;
struct entity;

struct camera *camera_create(void);
void camera_destroy(struct camera *camera);

void camera_move(struct camera *camera, float dx, float dy, float dz);
void camera_rotate(struct camera *camera, float dx, float dy, float dz);
void camera_change_pitch(struct camera *camera, float delta);
void camera_change_angle_around_pivot(struct camera *camera, float delta);
void camera_change_zoom(struct camera *camera, float delta);
void camera_relative_to_entity(struct camera *camera, struct entity *entity);

struct matrix4f *camera_view_matrix(struct camera *camera);

#endif
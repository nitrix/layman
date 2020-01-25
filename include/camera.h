#ifndef LEARNGL_CAMERA_H
#define LEARNGL_CAMERA_H

struct camera;

struct camera *camera_create(void);
void camera_destroy(struct camera *camera);

void camera_move(struct camera *camera, float dx, float dy, float dz);
struct matrix4f *camera_view_matrix(struct camera *camera);

#endif
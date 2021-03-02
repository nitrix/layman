#ifndef LAYMAN_PUBLIC_CAMERA_H
#define LAYMAN_PUBLIC_CAMERA_H

struct layman_camera *layman_camera_create(void);
void layman_camera_destroy(struct layman_camera *camera);

void layman_camera_translation(struct layman_camera *, float x, float y, float z);
void layman_camera_rotation(struct layman_camera *, float x, float y, float z);

#endif

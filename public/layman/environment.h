#ifndef LAYMAN_PUBLIC_ENVIRONMENT_H
#define LAYMAN_PUBLIC_ENVIRONMENT_H

#include "window.h"

struct layman_environment *layman_environment_create_from_hdr(const struct layman_window *window, const char *filepath);
void layman_environment_destroy(struct layman_environment *environment);

void renderCube(); // FIXME: MOVE ME SOMEWHERE!

#endif

#ifndef LAYMAN_ENVIRONMENT_H
#define LAYMAN_ENVIRONMENT_H

struct layman_environment *layman_environment_create_from_hdr(const char *filepath);
void layman_environment_destroy(struct layman_environment *environment);

#endif

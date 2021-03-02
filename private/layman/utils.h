#ifndef LAYMAN_PRIVATE_UTILS_H
#define LAYMAN_PRIVATE_UTILS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef thread_local
#define thread_local _Thread_local
#endif

#define UNUSED(x) (void) (x)

#include <string.h>
#define VEC3_ASSIGN(target, x, y, z) memcpy(target, (vec3) { x, y, z}, sizeof (target));
#define VEC4_ASSIGN(target, x, y, z, w) memcpy(target, (vec4) { x, y, z, w}, sizeof (target));

#define ARRAY_COUNT(x) (sizeof (x) / sizeof (x)[0])

#endif

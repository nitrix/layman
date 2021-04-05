#ifndef UTILS_H
#define UTILS_H

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
#define ARRAY_COUNT(x) (sizeof (x) / sizeof (x)[0])
#define TO_STR(x) #x
#define EVAL_TO_STR(x) TO_STR(x)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define DEFAULT_TITLE "Layman Game Engine"

void utils_render_cube(void);

#endif

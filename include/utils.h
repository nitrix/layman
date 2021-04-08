#ifndef UTILS_H
#define UTILS_H

#define DEFAULT_TITLE "Layman Game Engine"
#define VERSION "1.0.0"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#define UNUSED(x) (void) (x)
#define ARRAY_COUNT(x) (sizeof (x) / sizeof (x)[0])
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void utils_render_cube(void);
void utils_render_line(vec3 from, vec3 to, mat4 transform, vec4 color);
void utils_render_ngon(vec3 origin, int n, float radius, mat4 transform, vec4 color);
struct entity *find_selected_entity(void);

#endif

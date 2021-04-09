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

#include "cglm/cglm.h"

void utils_render_cube(void);
void utils_render_line(vec3 from, vec3 to, mat4 transform, vec4 color);
void utils_render_ngon(int n, float radius, mat4 transform, vec4 color);
float utils_closest_distance_between_two_rays(vec3 r1_origin, vec3 r1_direction, vec3 r2_origin, vec3 r2_direction, float *d1, float *d2);
float utils_closest_distance_between_ray_and_circle(vec3 ray_origin, vec3 ray_direction, vec3 circle_origin, vec3 circle_orientation, float circle_radius, vec3 closest);
bool utils_ray_plane_intersection(vec3 plane_origin, vec3 plane_normal, vec3 ray_origin, vec3 ray_direction, float *t);
struct entity *find_selected_entity(void);

#endif

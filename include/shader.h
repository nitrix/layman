#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

#include "matrix.h"
#include "vector.h"
#include "toolkit.h"

struct shader;

struct shader *shader_load_by_name(const char *name);
void shader_destroy(struct shader *shader);

void shader_use(struct shader *shader);
void shader_validate(struct shader *shader);

void shader_bind_uniform_bool(struct shader *shader, char *name, bool b);
void shader_bind_uniform_float(struct shader *shader, char *name, float f);
void shader_bind_uniform_vec3f(struct shader *shader, char *name, struct vector3f v);
void shader_bind_uniform_matrix4f(struct shader *shader, char *name, struct matrix4f m);

#endif

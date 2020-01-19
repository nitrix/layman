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

void shader_bind_uniforms(struct shader *shader, struct matrix4f *transformation, struct matrix4f *projection, struct matrix4f *view);

#endif

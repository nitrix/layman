#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

#include "matrix.h"
#include "vector.h"
#include "toolkit.h"
#include "light.h"
#include "entity.h"

struct shader;

struct shader *shader_load_by_name(const char *name);
void shader_destroy(struct shader *shader);

void shader_use(struct shader *shader);
void shader_validate(struct shader *shader);

void shader_bind_uniform_projection(struct shader *shader, struct matrix4f *projection);
void shader_bind_uniform_view(struct shader *shader, struct matrix4f *view);
void shader_bind_uniform_light(struct shader *shader, struct light *light);
void shader_bind_uniform_entity(struct shader *shader, struct entity *entity);

#endif

#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

#include "matrix.h"
#include "vector.h"
#include "toolkit.h"
#include "light.h"
#include "model.h"
#include "material.h"

#include <glad/glad.h>

struct shader {
    char *name;

    GLuint program_id;
    GLuint vertex_shader_id;
    GLuint fragment_shader_id;

    GLint uniform_model;
    GLint uniform_view;
    GLint uniform_projection;

    GLint uniform_light_position;
    GLint uniform_light_ambient;
    GLint uniform_light_diffuse;
    GLint uniform_light_specular;

    GLint uniform_material_ambient;
    GLint uniform_material_diffuse;
    GLint uniform_material_specular;
    GLint uniform_material_shininess;
};

void shader_destroy(struct shader *shader);

void shader_use(struct shader *shader);
void shader_validate(struct shader *shader);

void shader_bind_uniform_model(struct shader *shader, const struct matrix4f *model);
void shader_bind_uniform_view(struct shader *shader, const struct matrix4f *view);
void shader_bind_uniform_projection(struct shader *shader, const struct matrix4f *projection);
void shader_bind_uniform_light(struct shader *shader, const struct light *light);
void shader_bind_uniform_material(struct shader *shader, const struct material *material);

#endif

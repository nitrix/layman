#include "shader.h"

void shader_bind_uniform_matrix4f(GLint location, const struct matrix4f *m) {
    float buffer[16] = {
        m->x1, m->x2, m->x3, m->x4,
        m->y1, m->y2, m->y3, m->y4,
        m->z1, m->z2, m->z3, m->z4,
        m->w1, m->w2, m->w3, m->w4,
    };

    glUniformMatrix4fv(location, 1, false, buffer);
}

void shader_bind_uniform_model(struct shader *shader, const struct matrix4f *model) {
    shader_bind_uniform_matrix4f(shader->uniform_model, model);
}

void shader_bind_uniform_view(struct shader *shader, const struct matrix4f *view) {
    shader_bind_uniform_matrix4f(shader->uniform_view, view);
}

void shader_bind_uniform_projection(struct shader *shader, const struct matrix4f *projection) {
    shader_bind_uniform_matrix4f(shader->uniform_projection, projection);
}

void shader_bind_uniform_light(struct shader *shader, const struct light *light) {
    // Light position
    glUniform3f(shader->uniform_light_position, light->position.x, light->position.y, light->position.z);

    // Light color
    glUniform3f(shader->uniform_light_color, light->color.x, light->color.y, light->color.z);
}

void shader_bind_uniform_specular(struct shader *shader, float shine_damper, float reflectivity) {
    glUniform1f(shader->uniform_shine_damper, shine_damper);
    glUniform1f(shader->uniform_reflectivity, reflectivity);
}

void shader_validate(struct shader *shader) {
    glValidateProgram(shader->program_id);

    GLint status = 0;

    // Verify the validation went well
    glGetProgramiv(shader->program_id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[1024];

        glGetProgramInfoLog(shader->program_id, sizeof log, NULL, log);
        fprintf(stderr, "Unable to validate shader program named `%s`!\n%s", shader->name, log);
    }
}

void shader_use(struct shader *shader) {
    glUseProgram(shader->program_id);
}

void shader_destroy(struct shader *shader) {
    glDetachShader(shader->program_id, shader->vertex_shader_id);
    glDetachShader(shader->program_id, shader->fragment_shader_id);
    glDeleteShader(shader->vertex_shader_id);
    glDeleteShader(shader->fragment_shader_id);
    glDeleteProgram(shader->program_id);

    free(shader->name);
    free(shader);
}

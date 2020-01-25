#include "math/matrix.h"
#include "model.h"
#include "toolkit.h"
#include "math/vector.h"
#include "light.h"
#include "entity.h"

#include <GL/glew.h>

struct shader {
    char *name;

    GLuint program_id;
    GLuint vertex_shader_id;
    GLuint fragment_shader_id;

    GLint uniform_transformation;
    GLint uniform_projection;
    GLint uniform_view;
    GLint uniform_light_position;
    GLint uniform_light_color;
    GLint uniform_shine_damper;
    GLint uniform_reflectivity;
};

GLuint _shader_load_one(const char *filepath, GLenum shader_type) {
    FILE *file = fopen(filepath, "rb");
    fseek(file, 0, SEEK_END);
    GLint length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = malloc(length);
    fread(content, 1, length, file);
    fclose(file);

    GLuint shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, (const char **) &content, &length);
    glCompileShader(shader_id);

    free(content);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar log[1024];
        glGetShaderInfoLog(shader_id, sizeof log, NULL, log);

        fprintf(stderr, "Unable to load shader `%s`!\n%s", filepath, log);

        return 0;
    }

    return shader_id;
}

struct shader *shader_load_by_name(const char *name) {
    struct shader *shader = malloc(sizeof *shader);

    if (!shader) {
        return NULL;
    }

    // Copy name
    shader->name = strdup(name);

    // Generate paths
    size_t size = 0;
    size = snprintf(NULL, 0, "shaders/%s/vertex.glsl", name);
    char vertex_path[size + 1];
    sprintf(vertex_path, "shaders/%s/vertex.glsl", name);
    size = snprintf(NULL, 0, "shaders/%s/fragment.glsl", name);
    char fragment_path[size + 1];
    sprintf(fragment_path, "shaders/%s/fragment.glsl", name);

    shader->vertex_shader_id = _shader_load_one(vertex_path, GL_VERTEX_SHADER);
    shader->fragment_shader_id = _shader_load_one(fragment_path, GL_FRAGMENT_SHADER);

    // Create the shader program
    shader->program_id = glCreateProgram();
    glAttachShader(shader->program_id, shader->vertex_shader_id);
    glAttachShader(shader->program_id, shader->fragment_shader_id);

    // Binding shader input variables to attributes.
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_VERTEX_COORDINATES, "position");
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_TEXTURE_COORDINATES, "texture_coords");
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_NORMALS, "normal");

    // Linking
    glLinkProgram(shader->program_id);

    // Verify the linking went well
    GLint status = 0;
    glGetProgramiv(shader->program_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[1024];

        glGetProgramInfoLog(shader->program_id, sizeof log, NULL, log);
        fprintf(stderr, "Unable to link shader program named `%s`!\n%s", name, log);

        // TODO: Cleanup shaders on failure
        return NULL;
    }

    // Find the uniforms
    shader->uniform_transformation = glGetUniformLocation(shader->program_id, "transformation");
    shader->uniform_projection = glGetUniformLocation(shader->program_id, "projection");
    shader->uniform_view = glGetUniformLocation(shader->program_id, "view");
    shader->uniform_light_position = glGetUniformLocation(shader->program_id, "light_position");
    shader->uniform_light_color = glGetUniformLocation(shader->program_id, "light_color");
    shader->uniform_shine_damper = glGetUniformLocation(shader->program_id, "shine_damper");
    shader->uniform_reflectivity = glGetUniformLocation(shader->program_id, "reflectivity");

    return shader;
}

void _shader_bind_uniform_matrix4f(GLint location, struct matrix4f *m) {
    float buffer[16] = {
        m->x1, m->x2, m->x3, m->x4,
        m->y1, m->y2, m->y3, m->y4,
        m->z1, m->z2, m->z3, m->z4,
        m->w1, m->w2, m->w3, m->w4,
    };

    glUniformMatrix4fv(location, 1, false, buffer);
}

void shader_bind_uniforms(struct shader *shader, struct matrix4f *projection, struct matrix4f *view) {
    _shader_bind_uniform_matrix4f(shader->uniform_projection, projection);
    _shader_bind_uniform_matrix4f(shader->uniform_view, view);
}

void shader_bind_uniforms_light(struct shader *shader, struct light *light) {
    // Light position
    struct vector3f *light_position = light_get_position(light);
    glUniform3f(shader->uniform_light_position, light_position->x, light_position->y, light_position->z);

    // Light color
    struct vector3f *light_color = light_get_color(light);
    glUniform3f(shader->uniform_light_color, light_color->x, light_color->y, light_color->z);
}

void shader_bind_uniforms_entity(struct shader *shader, struct entity *entity) {
    // Transformation
    struct matrix4f transformation = matrix_create_from_transformation(entity->position, entity->rotation, entity->scale);
    _shader_bind_uniform_matrix4f(shader->uniform_transformation, &transformation);

    // Specular
    glUniform1f(shader->uniform_shine_damper, entity->shine_damper);
    glUniform1f(shader->uniform_reflectivity, entity->reflectivity);
}

void shader_bind_uniforms_specular(struct shader *shader, float shine_damper, float reflectivity) {

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

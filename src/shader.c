#include "toolkit.h"
#include <GL/glew.h>

struct shader {
    GLuint program_id;
    GLuint vertex_shader_id;
    GLuint fragment_shader_id;
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

    glShaderSource(shader_id, 1, &content, &length);
    glCompileShader(shader_id);

    free(content);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];

        glGetShaderInfoLog(shader_id, length, NULL, log);

        fprintf(stderr, "Unable to load shader `%s`!\n%s", filepath, log);

        return 0;
    }

    return shader_id;
}

struct shader *shader_load(const char *vertex_path, const char *fragment_path) {
    struct shader *shader = malloc(sizeof *shader);

    if (!shader) {
        return NULL;
    }

    // TODO: It would be nice to be able to load a "shader" by name, which
    // corresponds to a directory of that name, containing the various GLSL shaders (vertex, fragment, geometry).

    shader->vertex_shader_id = _shader_load_one(vertex_path, GL_VERTEX_SHADER);
    shader->fragment_shader_id = _shader_load_one(fragment_path, GL_FRAGMENT_SHADER);

    // Create the shader program
    shader->program_id = glCreateProgram();
    glAttachShader(shader->program_id, shader->vertex_shader_id);
    glAttachShader(shader->program_id, shader->fragment_shader_id);
    glLinkProgram(shader->program_id);
    glValidateProgram(shader->program_id);

    // Verify the validation went well
    GLint status = 0;
    glGetProgramiv(shader->program_id, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader->program_id, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];

        glGetProgramInfoLog(shader->program_id, length, NULL, log);
        fprintf(stderr, "Unable to validate shader program!\n%s", log);

        // TODO: Cleanup shaders on failure
        return NULL;
    }

    return shader;
}

void shader_use(struct shader *shader) {
    glUseProgram(shader->program_id);
}

void shader_unuse(struct shader *shader) {
    TK_UNUSED(shader);
    glUseProgram(0);
}

void shader_bind(struct shader *shader, int index, const char *name) {
    glBindAttribLocation(shader->program_id, index, name);
}

void shader_destroy(struct shader *shader) {
    glDetachShader(shader->program_id, shader->vertex_shader_id);
    glDetachShader(shader->program_id, shader->fragment_shader_id);
    glDeleteShader(shader->vertex_shader_id);
    glDeleteShader(shader->fragment_shader_id);
    glDeleteProgram(shader->program_id);

    free(shader);
}
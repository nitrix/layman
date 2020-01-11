#include "model.h"
#include "toolkit.h"

#include <GL/glew.h>

struct shader {
    char *name;

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

    // Binding shader input variables to attributes
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_VERTEX_COORDINATES, "position");
    glBindAttribLocation(shader->program_id, MODEL_ATTRIBUTE_TEXTURE_COORDINATES, "texture_coords");

    return shader;
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

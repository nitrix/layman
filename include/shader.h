#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

struct shader;

struct shader *shader_load(const char *vertex_path, const char *fragment_path);
void shader_destroy(struct shader *shader);

void shader_use(struct shader *shader);
void shader_unuse(struct shader *shader);
void shader_bind(struct shader *shader, int index, const char *name);

#endif
#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

struct shader;

struct shader *shader_load_by_name(const char *name);
void shader_destroy(struct shader *shader);

void shader_use(struct shader *shader);
void shader_bind(struct shader *shader, int index, const char *name);
void shader_validate(struct shader *shader);

#endif

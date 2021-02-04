#ifndef LAYMAN_SHADER_H
#define LAYMAN_SHADER_H

// TODO: Documentation.
struct layman_shader *layman_shader_load_from_files(const char *vertex_filepath, const char *fragment_filepath, const char *compute_filepath);

// TODO: Documentation.
void layman_shader_destroy(struct layman_shader *shader);

// TODO: Documentation.
void layman_shader_bind_uniform_material(const struct layman_shader *shader, const struct layman_material *material);

// TODO: Documentation.
void layman_shader_bind_uniform_camera(const struct layman_shader *shader, const struct layman_camera *camera);

// TODO: Documentation.
void layman_shader_bind_uniform_lights(const struct layman_shader *shader, const struct layman_light **lights, size_t count);

#endif

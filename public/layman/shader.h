#ifndef LAYMAN_PUBLIC_SHADER_H
#define LAYMAN_PUBLIC_SHADER_H

// TODO: Documentation.
struct layman_shader *layman_shader_load_from_files(const char *vertex_filepath, const char *fragment_filepath, const char *compute_filepath);

// TODO: Documentation.
struct layman_shader *layman_shader_load_from_memory(const unsigned char *vertex_content, size_t vertex_length, const unsigned char *fragment_content, size_t fragment_length, const unsigned char *compute_content, size_t compute_length);

// TODO: Documentation.
void layman_shader_destroy(struct layman_shader *shader);

// TODO: Documentation.
void layman_shader_bind_uniform_material(const struct layman_shader *shader, const struct layman_material *material);

// TODO: Documentation.
void layman_shader_bind_uniform_camera(const struct layman_shader *shader, const struct layman_camera *camera);

// TODO: Documentation.
void layman_shader_bind_uniform_lights(const struct layman_shader *shader, const struct layman_light **lights, size_t count);

// TODO: Documentation.
void layman_shader_bind_uniform_environment(const struct layman_shader *shader, const struct layman_environment *environment);

#endif

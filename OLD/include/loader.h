#ifndef LEARNGL_LOADER_H
#define LEARNGL_LOADER_H

#include "model.h"
#include "texture.h"
#include "shader.h"
#include "toolkit.h"
#include "vector.h"

#include <glad/glad.h>

struct model *loader_load_model(const char *filepath);
struct texture *loader_load_texture(const char *filepath);
struct shader *loader_load_shader(const char *name);

#endif

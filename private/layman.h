#ifndef LAYMAN_PRIVATE_LAYMAN_H
#define LAYMAN_PRIVATE_LAYMAN_H

// Get the incomplete (and sometimes complete) public type definitions.
#include "../public/layman.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "glad/glad.h"
#include "gltf.h"
#include "stb_image.h"

#define MAX_LIGHTS 4

#include "layman/camera.h"
#include "layman/entity.h"
#include "layman/environment.h"
#include "layman/framebuffer.h"
#include "layman/light.h"
#include "layman/material.h"
#include "layman/mesh.h"
#include "layman/model.h"
#include "layman/renderer.h"
#include "layman/scene.h"
#include "layman/shader.h"
#include "layman/texture.h"
#include "layman/window.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef thread_local
#define thread_local _Thread_local
#endif

#define TO_STR(x) #x
#define EVAL_TO_STR(x) TO_STR(x)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define UNUSED(x) (void) (x)
#define VEC3_ASSIGN(target, x, y, z) memcpy(target, (vec3) { x, y, z}, sizeof (target));
#define VEC4_ASSIGN(target, x, y, z, w) memcpy(target, (vec4) { x, y, z, w}, sizeof (target));

#endif

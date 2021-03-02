#ifndef LAYMAN_PRIVATE_LAYMAN_H
#define LAYMAN_PRIVATE_LAYMAN_H

/*
 *   ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗
 *   ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║
 *   ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║      ██║
 *   ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝
 *   ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗      ██╗
 *   ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝      ╚═╝
 *
 *    IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT. IMPORTANT.
 *
 *
 *                                   For internal use only. Do NOT include directly.
 *
 *
 * When using the CMake configuration, the `layman` library will provide you only the public headers.
 * If you think something from the private API should be made public, then please open an issue instead and we'll work
 * it out.
 *
 * The internal API changes too frequently, steals plenty of useful identifiers and requires intimate knowledge of the
 * state of the various rendering pipelines to not majorly screw up.
 *
 * Here follows a guaranteed headache that you really want to avoid. You have been warned.
 */

// Supplement the missing private definitions from the public ones (sometimes complete, sometimes incomplete).
// Externally, the "layman.h" include points to the public folder, internally, the "layman.h" include points to the
// private folder. By including this file relatively, we're able to force the lookup to be the public folder.
#include "../public/layman.h"

// Continue normally with the private definitions.
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
#include "layman/utils.h"
#include "layman/window.h"

#endif

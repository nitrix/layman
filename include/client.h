#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glad/glad.h"
#include "gltf.h"
#include "incbin.h"
#include "stb_image.h"

#include "camera.h"
#include "entity.h"
#include "environment.h"
#include "framebuffer.h"
#include "light.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "modelmanager.h"
#include "renderer.h"
#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "ui.h"
#include "utils.h"
#include "window.h"

struct client {
	struct window window;
	struct renderer renderer;
	struct camera camera;
	struct scene scene;
	struct ui ui;
};

extern struct client client;

#endif

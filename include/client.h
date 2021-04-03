#ifndef CLIENT_H
#define CLIENT_H

#include "camera.h"
#include "renderer.h"
#include "scene.h"
#include "ui.h"
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

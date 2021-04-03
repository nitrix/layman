#ifndef STATE_H
#define STATE_H

#include "ui.h"
#include "window.h"

struct state {
	struct window window;
	struct renderer *renderer;
	struct camera camera;
	struct scene scene;
	struct environment *environment;
	struct ui ui;
};

extern struct state state;

#endif

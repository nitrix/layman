#ifndef STATE_H
#define STATE_H

struct state {
	struct window *window;
	struct renderer *renderer;
	struct camera *camera;
	struct scene *scene;
	struct environment *environment;
};

extern struct state state;

#endif

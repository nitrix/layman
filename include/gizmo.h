#ifndef GIZMO_H
#define GIZMO_H

enum gizmo_mode {
	GIZMO_MODE_NONE,
	GIZMO_MODE_TRANSLATION,
	GIZMO_MODE_ROTATION,
	GIZMO_MODE_SCALE,
};

struct gizmo {
	enum gizmo_mode mode;
};

void gizmo_init(struct gizmo *gizmo);
void gizmo_fini(struct gizmo *gizmo);
void gizmo_render(struct gizmo *gizmo);

#endif

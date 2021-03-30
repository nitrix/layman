#ifndef UI_H
#define UI_H

#include "cimgui.h"
#include "cimgui_impl.h"

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	struct renderer *renderer;

	bool show;
	bool show_demo;
	struct texture *logo;
};

struct ui *ui_create(struct renderer *renderer);
void ui_destroy(struct ui *ui);
void ui_render(struct ui *ui);

#endif

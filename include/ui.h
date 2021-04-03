#ifndef UI_H
#define UI_H

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	struct renderer *renderer;

	bool show;
	bool show_demo;
	bool show_model_editor;
	bool show_scene_editor;
	struct texture *logo;
};

struct ui *ui_create(struct renderer *renderer);
void ui_destroy(struct ui *ui);
void ui_render(struct ui *ui);

#endif

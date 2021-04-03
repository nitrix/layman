#ifndef UI_H
#define UI_H

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	bool show;
	bool show_imgui_demo;
	bool show_model_manager;
	bool show_scene_editor;
	bool show_debugging_tools;

	struct texture logo;
};

bool ui_init(struct ui *ui);
void ui_fini(struct ui *ui);
void ui_render(struct ui *ui);

#endif

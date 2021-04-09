#ifndef UI_H
#define UI_H

#include "gizmo.h"
#include "texture.h"

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	bool show;
	bool show_imgui_demo;
	bool show_scene_editor;
	bool show_debug_tools;
	bool show_settings;
	bool show_debug_camera;
	bool show_about;

	uint32_t selected_entity_id;

	struct texture assets_logo_png;
	struct texture assets_gear_png;
	struct texture assets_bug_png;
	struct texture assets_question_png;
	struct texture assets_cube_png;

	struct gizmo gizmo;
};

bool ui_init(struct ui *ui);
void ui_fini(struct ui *ui);
void ui_render(struct ui *ui);

#endif

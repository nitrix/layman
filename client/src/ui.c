#include "incbin.h"
#include "renderer.h"
#include "state.h"
#include "texture.h"
#include "ui.h"
#include <stdlib.h>

INCBIN(assets_logo_white_png, "../../assets/logo_white.png");

float step_size = 0.01;

struct ui *ui_create(struct renderer *renderer) {
	struct ui *ui = malloc(sizeof *ui);

	if (!ui) {
		return NULL;
	}

	ui->ig_context = igCreateContext(NULL);
	ui->ig_io = igGetIO();
	ui->ig_io->IniFilename = NULL;
	ui->show_demo = false;
	ui->renderer = renderer;
	ui->show_model_editor = false;
	ui->show_scene_editor = false;
	ui->show = false;

	ImGui_ImplGlfw_InitForOpenGL(renderer->window->glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");
	igStyleColorsDark(NULL);

	ui->logo = texture_create_from_memory(TEXTURE_KIND_IMAGE, assets_logo_white_png_data, assets_logo_white_png_size);
	if (!ui->logo) {
		// TODO: Cleanup.
		return NULL;
	}

	return ui;
}

void ui_destroy(struct ui *ui) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(ui->ig_context);

	texture_destroy(ui->logo);
	free(ui);
}

void ui_render_model_editor(struct ui *ui) {
	if (!ui->show_model_editor) {
		return;
	}

	if (igBegin("Model Editor", &ui->show_model_editor, ImGuiWindowFlags_None)) {
		if (igBeginTabBar("##model-editor-tab-bar", ImGuiTabBarFlags_None)) {
			static bool open[2] = {true, true};

			if (igBeginTabItem("DamagedHelmet.glb", &open[0], ImGuiTabItemFlags_None)) {
				igEndTabItem();
			}

			if (igBeginTabItem("BoomBox.glb", &open[1], ImGuiTabItemFlags_None)) {
				igEndTabItem();
			}

			if (igTabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
				// active_tabs.push_back(next_tab_id++); // Add new tab
			}

			igEndTabBar();
		}

		igText("Hello World!");
	}

	igEnd();
}

void ui_render_fps_tracker(struct ui *ui) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	const ImGuiViewport *viewport = igGetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + 10;
	window_pos.y = work_pos.y + 10;
	window_pos_pivot.x = 0.0f;
	window_pos_pivot.y = 0.0f;
	igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	igSetNextWindowBgAlpha(0.35f); // Transparent background
	igSetNextWindowSizeConstraints((ImVec2) { 100, -1}, (ImVec2) { -1, -1}, NULL, NULL);

	if (igBegin("FPS counter", NULL, window_flags)) {
		if (ui->renderer->fps_history_total_count) {
			igText("FPS: %d\n", (int) ui->renderer->fps_history[(ui->renderer->fps_history_total_count - 1) % FPS_HISTORY_MAX_COUNT]);
		} else {
			igText("FPS: Unknown\n");
		}

		float scale_max = ui->renderer->fps_history_highest + (ui->renderer->fps_history_highest * 0.25);
		igPushStyleColorVec4(ImGuiCol_FrameBg, (ImVec4) { 0, 0, 0, 0.25});
		igPlotHistogramFloatPtr("", ui->renderer->fps_history, FPS_HISTORY_MAX_COUNT, ui->renderer->fps_history_total_count, NULL, 0, scale_max, (ImVec2) { 100, 50}, sizeof (float));
		igPopStyleColor(1);
	}

	igEnd();
}

void prepare_centered_text(const char *text) {
	float window_width = igGetWindowWidth();

	ImVec2 dimension;
	igCalcTextSize(&dimension, text, NULL, false, -1);

	igSetCursorPosX(window_width / 2 - dimension.x / 2);
}

void ui_render_main_navigation(struct ui *ui) {
	// Minimum width for then next window.
	size_t window_minimum_width = 300;
	igSetNextWindowSize((ImVec2) { window_minimum_width, -1}, 0);

	// Center the next window.
	igSetNextWindowPos((ImVec2) { ui->renderer->viewport_width / 2, ui->renderer->viewport_height / 2}, ImGuiCond_Once, (ImVec2) { 0.5, 0.5});

	// The window.
	if (igBegin("Layman Game Engine", NULL, ImGuiWindowFlags_NoCollapse)) {
		float width = igGetWindowWidth();

		// Display the logo centered.
		igSetCursorPosX((width - ui->logo->width / 2) / 2);
		ImTextureID texture_id = (void *) (uintptr_t) ui->logo->gl_id; // Absolutely gross.
		igImage(texture_id, (ImVec2) { ui->logo->width / 2, ui->logo->height / 2}, (ImVec2) { 0, 0}, (ImVec2) { 1, 1}, (ImVec4) { 1, 1, 1, 1}, (ImVec4) { 0, 0, 0, 0});

		igSeparator();

		// All the buttons.
		igButton("World Editor", (ImVec2) { -1, 0});
		if (igButton("Model Editor", (ImVec2) { -1, 0})) {
			ui->show_model_editor = true;
		}

		igButton("Script Editor", (ImVec2) { -1, 0});
		igButton("Settings", (ImVec2) { -1, 0});
		igButton("About", (ImVec2) { -1, 0});

		igSeparator();

		// Footer.
		char *version = "Version 1.0.0 (build 5952a4c)";
		prepare_centered_text(version);
		igTextDisabled(version);
	}

	igEnd();
}

void ui_render_scene_editor(struct ui *ui) {
	igSetNextWindowSize((ImVec2) { 350, 340}, ImGuiCond_Once);

	if (igBegin("Scene editor", &ui->show_scene_editor, ImGuiWindowFlags_None)) {
		igAlignTextToFramePadding();
		igText("Filter:");
		igSameLine(0, -1);

		static char buf[1024] = {0};
		igSetNextItemWidth(-1);
		igInputText("##scene-search", buf, sizeof buf, ImGuiInputTextFlags_None, NULL, NULL);

		igSeparator();

		igPushStyleColorVec4(ImGuiCol_ChildBg, (ImVec4) { 0.16, 0.29, 0.48, 0.54});

		igBeginChildStr("##scene-entities", (ImVec2) { -1, 200}, false, ImGuiWindowFlags_None);

		static struct entity *selected_entity = NULL;
		bool found_selected_entity = false;

		for (size_t i = 0; i < state.scene->entity_count; i++) {
			struct entity *entity = state.scene->entities[i];

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;

			if (entity == selected_entity) {
				flags |= ImGuiTreeNodeFlags_Selected;
				found_selected_entity = true;
			}

			char buffer[1024];
			snprintf(buffer, sizeof buffer, "##entity-index-%d", i);

			if (igTreeNodeExStrStr(buffer, flags, "%s", entity->model->name)) {
				igTreePop();
			}

			if (igIsItemClicked(ImGuiMouseButton_Left)) {
				selected_entity = entity;
				found_selected_entity = true;
			}
		}

		igEndChild();

		igPopStyleColor(1);

		// We have to clear the selected entity if it is no longer in the scene to avoid invalid memory access.
		if (!found_selected_entity) {
			selected_entity = NULL;
		}

		igSeparator();

		if (selected_entity) {
			if (igButton("R##reset-translation", (ImVec2) { 0, 0})) {
				glm_vec3_zero(selected_entity->translation);
			}

			igSameLine(0, -1);
			igDragFloat3("Translation", selected_entity->translation, step_size, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None);

			if (igButton("R##reset-rotation", (ImVec2) { 0, 0})) {
				glm_vec3_zero(selected_entity->rotation);
			}

			igSameLine(0, -1);
			igDragFloat3("Rotation", selected_entity->rotation, step_size, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None);

			if (igButton("R##reset-scale", (ImVec2) { 0, 0})) {
				selected_entity->scale = 1;
			}

			igSameLine(0, -1);
			igDragFloat("Scale", &selected_entity->scale, step_size, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None);
		} else {
			igText("Select an entity.");
		}
	}

	igEnd();
}

void ui_render_debug_window(struct ui *ui) {
	if (igBegin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		igCheckbox("ImGUI demo window", &ui->show_demo);

		if (igCheckbox("Wireframe mode", &ui->renderer->wireframe)) {
			renderer_wireframe(ui->renderer, ui->renderer->wireframe);
		}

		igCheckbox("Scene editor", &ui->show_scene_editor);

		if (igCheckbox("Fullscreen", &state.window->fullscreen)) {
			window_fullscreen(state.window, state.window->fullscreen);
		}
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (ui->show) {
		// ui_render_main_navigation(ui);
		// ui_render_model_editor(ui);
		// ui_render_fps_tracker(ui);
		ui_render_debug_window(ui);

		if (ui->show_scene_editor) {
			ui_render_scene_editor(ui);
		}

		if (ui->show_demo) {
			igShowDemoWindow(&ui->show_demo);
		}
	}

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

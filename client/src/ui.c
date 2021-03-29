#include "cimgui.h"
#include "cimgui_impl.h"
#include "incbin.h"
#include "renderer.h"
#include "texture.h"
#include "ui.h"
#include <stdlib.h>

INCBIN(assets_logo_white_png, "../../assets/logo_white.png");

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	struct renderer *renderer;

	bool show_demo;
	struct texture *logo;
};

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
		igButton("Model Editor", (ImVec2) { -1, 0});
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

void ui_render_sidebar(struct ui *ui) {
	igBegin("Foo", NULL, ImGuiWindowFlags_NoCollapse);

	igCheckbox("Demo window", &ui->show_demo);

	if (igCheckbox("Wireframe", &ui->renderer->wireframe)) {
		renderer_wireframe(ui->renderer, ui->renderer->wireframe);
	}

	igSeparator();

	if (igBeginTabBar("tab-bar", ImGuiTabBarFlags_None)) {
		if (igBeginTabItem("Scene", NULL, ImGuiTabItemFlags_None)) {
			igAlignTextToFramePadding();
			igText("Filter:");
			igSameLine(0, -1);

			static char buf[1024] = {0};
			igSetNextItemWidth(-1);
			igInputText("##scene-search", buf, sizeof buf, ImGuiInputTextFlags_None, NULL, NULL);

			if (igBeginListBox("##scene-entities", (ImVec2) { -1, -1})) {
				char *entities[] = {"DamagedHelmet 1", "DamagedHelmet 2"};
				static int selected = -1;

				for (int i = 0; i < 2; i++) {
					if (igSelectableBool(entities[i], i == selected, ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_SpanAvailWidth, (ImVec2) { 0, 0})) {
						selected = i;
					}
				}

				igEndListBox();
			}

			igEndTabItem();
		}

		if (igBeginTabItem("Settings", NULL, ImGuiTabItemFlags_NoCloseButton)) {
			igText("Bar");

			igEndTabItem();
		}

		igEndTabBar();
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (ui->show_demo) {
		igShowDemoWindow(NULL);
	}

	ui_render_main_navigation(ui);
	// ui_render_fps_tracker(ui);
	// ui_render_sidebar(ui);

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

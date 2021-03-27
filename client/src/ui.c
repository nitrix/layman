#include "cimgui.h"
#include "cimgui_impl.h"
#include "renderer.h"
#include "ui.h"
#include <stdlib.h>

struct ui {
	// UI via (c)imgui, aka ig.
	struct ImGuiContext *ig_context;
	struct ImGuiIO *ig_io;

	struct renderer *renderer;
};

struct ui *ui_create(struct renderer *renderer) {
	struct ui *ui = malloc(sizeof *ui);

	if (!ui) {
		return NULL;
	}

	ui->ig_context = igCreateContext(NULL);
	ui->ig_io = igGetIO();
	ui->ig_io->IniFilename = NULL;

	ImGui_ImplGlfw_InitForOpenGL(renderer->window->glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");
	igStyleColorsDark(NULL);

	ui->renderer = renderer;

	return ui;
}

void ui_destroy(struct ui *ui) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(ui->ig_context);

	free(ui);
}

void ui_render_fps_tracker(struct ui *ui) {
	static bool open;

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

	if (igBegin("FPS counter", &open, window_flags)) {
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

void ui_render_sidebar(struct ui *ui) {
	bool open;

	igSetNextWindowSize((ImVec2) { 100, 100}, 0);
	igBegin("Foo", &open, ImGuiWindowFlags_NoResize);

	if (igCheckbox("Wireframe", &ui->renderer->wireframe)) {
		renderer_wireframe(ui->renderer, ui->renderer->wireframe);
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	// igShowDemoWindow(NULL);
	ui_render_fps_tracker(ui);
	ui_render_sidebar(ui);

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

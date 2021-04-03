#include "client.h"

INCBIN(assets_logo_white_png, "../../assets/logo_white.png");

static float step_size = 0.005;

bool ui_init(struct ui *ui) {
	ui->ig_context = igCreateContext(NULL);
	ui->ig_io = igGetIO();
	ui->ig_io->IniFilename = NULL;
	ui->show_imgui_demo = false;
	ui->show_model_manager = false;
	ui->show_scene_editor = false;
	ui->show_debugging_tools = false;
	ui->show = false;

	ImGui_ImplGlfw_InitForOpenGL(state.renderer->window->glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");
	igStyleColorsDark(NULL);

	if (!texture_init_from_memory(&ui->logo, TEXTURE_KIND_IMAGE, assets_logo_white_png_data, assets_logo_white_png_size)) {
		// TODO: Cleanup imgui?
		return false;
	}

	return true;
}

void ui_fini(struct ui *ui) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(ui->ig_context);
	texture_fini(&ui->logo);
}

/*
    static void render_model_manager(struct ui *ui) {
        if (igBegin("Model Manager", &ui->show_model_manager, ImGuiWindowFlags_None)) {
                if (igBeginTabBar("##model-manager-tab-bar", ImGuiTabBarFlags_None)) {
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
 */

static void prepare_centered_text(const char *text) {
	float window_width = igGetWindowWidth();

	ImVec2 dimension;
	igCalcTextSize(&dimension, text, NULL, false, -1);

	igSetCursorPosX(window_width / 2 - dimension.x / 2);
}

static void render_debugging_tools(struct ui *ui) {
	if (igBegin("Debugging tools", &ui->show_debugging_tools, ImGuiWindowFlags_None)) {
		igText("Cursor position: %f %f", state.window.cursor_pos_x, state.window.cursor_pos_y);
		igText("Mouse picked: %u", state.renderer->mousepicking_entity_id);

		igSeparator();

		if (igCheckbox("Wireframe mode", &state.renderer->wireframe)) {
			renderer_wireframe(state.renderer, state.renderer->wireframe);
		}

		if (igCheckbox("Fullscreen", &state.window.fullscreen)) {
			window_fullscreen(&state.window, state.window.fullscreen);
		}

		igCheckbox("ImGUI Demo Window", &ui->show_imgui_demo);
	}

	igEnd();
}

static void render_main_navigation(struct ui *ui) {
	// Minimum width for then next window.
	size_t window_minimum_width = 300;
	igSetNextWindowSize((ImVec2) { window_minimum_width, -1}, 0);

	// Center the next window.
	igSetNextWindowPos((ImVec2) { state.renderer->viewport_width / 2, state.renderer->viewport_height / 2}, ImGuiCond_Once, (ImVec2) { 0.5, 0.5});

	// The window.
	if (igBegin("Layman Game Engine", NULL, ImGuiWindowFlags_NoCollapse)) {
		float width = igGetWindowWidth();

		// Display the logo centered.
		igSetCursorPosX((width - ui->logo.width / 2) / 2);
		ImTextureID texture_id = (void *) (uintptr_t) ui->logo.gl_id; // Absolutely gross.
		igImage(texture_id, (ImVec2) { ui->logo.width / 2, ui->logo.height / 2}, (ImVec2) { 0, 0}, (ImVec2) { 1, 1}, (ImVec4) { 1, 1, 1, 1}, (ImVec4) { 0, 0, 0, 0});

		igSeparator();

		// All the buttons.
		if (igButton("Model Manager", (ImVec2) { -1, 0})) {
			ui->show_model_manager = true;
		}

		if (igButton("Scene Editor", (ImVec2) { -1, 0})) {
			ui->show_scene_editor = true;
		}

		if (igButton("Debugging tools", (ImVec2) { -1, 0})) {
			ui->show_debugging_tools = true;
		}

		igButton("Settings", (ImVec2) { -1, 0});
		igButton("About", (ImVec2) { -1, 0});

		igSeparator();

		// Footer.
		char *version = "Version 1.0.0 (build 5952a4c)"; // FIXME: Needs to stay up-to-date.
		prepare_centered_text(version);
		igTextDisabled(version);
	}

	igEnd();
}

static void render_scene_editor(struct ui *ui) {
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

		for (size_t i = 0; i < state.scene.entity_count; i++) {
			struct entity *entity = state.scene.entities[i];

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
				glm_quat_identity(selected_entity->rotation);
			}

			igSameLine(0, -1);

			vec3 old_euler_rotation, new_euler_rotation;
			mat4 rotation;

			glm_quat_mat4(selected_entity->rotation, rotation);
			glm_euler_angles(rotation, old_euler_rotation);
			glm_vec3_copy(old_euler_rotation, new_euler_rotation);

			if (igDragFloat3("Rotation", new_euler_rotation, step_size, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
				vec3 delta_euler_rotation;
				glm_vec3_sub(new_euler_rotation, old_euler_rotation, delta_euler_rotation);

				versor rx, ry, rz, r;
				glm_quat(rx, delta_euler_rotation[0], 1, 0, 0);
				glm_quat(ry, delta_euler_rotation[1], 0, 1, 0);
				glm_quat(rz, delta_euler_rotation[2], 0, 0, 1);
				glm_quat_mul(rx, ry, r);
				glm_quat_mul(r, rz, r);

				glm_quat_mul(selected_entity->rotation, r, selected_entity->rotation);
			}

			if (igButton("R##reset-scale", (ImVec2) { 0, 0})) {
				selected_entity->scale = 1;
			}

			igSameLine(0, -1);
			igDragFloat("Scale", &selected_entity->scale, step_size, 0, FLT_MAX, "%f", ImGuiSliderFlags_None);
		} else {
			igText("Select an entity.");
		}
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (ui->show) {
		render_main_navigation(ui);
		// render_model_editor(ui);

		if (ui->show_debugging_tools) {
			render_debugging_tools(ui);
		}

		if (ui->show_scene_editor) {
			render_scene_editor(ui);
		}

		if (ui->show_imgui_demo) {
			igShowDemoWindow(&ui->show_imgui_demo);
		}
	}

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

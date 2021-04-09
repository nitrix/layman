#include "cimgui.h"
#include "cimgui_impl.h"
#include "client.h"
#include "entity.h"
#include "incbin.h"
#include "model.h"
#include "ui.h"

INCBIN(assets_logo_png, "../assets/logo.png");
INCBIN(assets_gear_png, "../assets/gear.png");
INCBIN(assets_bug_png, "../assets/bug.png");
INCBIN(assets_question_png, "../assets/question.png");
INCBIN(assets_cube_png, "../assets/cube.png");

static float step_size = 0.005;

bool ui_init(struct ui *ui) {
	ui->ig_context = igCreateContext(NULL);
	ui->ig_io = igGetIO();
	ui->ig_io->IniFilename = NULL;

	ui->show = false;
	ui->show_imgui_demo = false;
	ui->show_scene_editor = false;
	ui->show_debug_tools = false;
	ui->show_settings = false;
	ui->show_debug_camera = false;
	ui->show_about = false;

	ui->selected_entity_id = 0; // FIXME: Does not belong here.

	ImGui_ImplGlfw_InitForOpenGL(client.window.glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");
	igStyleColorsDark(NULL);

	texture_init_from_memory(&ui->assets_logo_png, TEXTURE_KIND_IMAGE, assets_logo_png_data, assets_logo_png_size);
	texture_init_from_memory(&ui->assets_gear_png, TEXTURE_KIND_IMAGE, assets_gear_png_data, assets_gear_png_size);
	texture_init_from_memory(&ui->assets_bug_png, TEXTURE_KIND_IMAGE, assets_bug_png_data, assets_bug_png_size);
	texture_init_from_memory(&ui->assets_question_png, TEXTURE_KIND_IMAGE, assets_question_png_data, assets_question_png_size);
	texture_init_from_memory(&ui->assets_cube_png, TEXTURE_KIND_IMAGE, assets_cube_png_data, assets_cube_png_size);

	gizmo_init(&ui->gizmo);

	return true;
}

void ui_fini(struct ui *ui) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(ui->ig_context);
	texture_fini(&ui->assets_logo_png);
	texture_fini(&ui->assets_gear_png);
	texture_fini(&ui->assets_bug_png);
	texture_fini(&ui->assets_question_png);
	texture_fini(&ui->assets_cube_png);
	gizmo_fini(&ui->gizmo);
}

static void center_next_window(void) {
	igSetNextWindowPos((ImVec2) { client.renderer.viewport_width / 2, client.renderer.viewport_height / 2}, ImGuiCond_Once, (ImVec2) { 0.5, 0.5});
}

static void prepare_centered_text(const char *text) {
	float window_width = igGetWindowWidth();

	ImVec2 dimension;
	igCalcTextSize(&dimension, text, NULL, false, -1);

	igSetCursorPosX(window_width / 2 - dimension.x / 2);
}

static void bring_window_to_focus_back(struct ui *ui, ImGuiWindow *window) {
	// Sorted in focus order, back to front.

	if (ui->ig_context->WindowsFocusOrder.Size > 1) {
		ImGuiWindow *carry = NULL;

		// If the first element is our window, no need to do anything else.
		if (ui->ig_context->WindowsFocusOrder.Data[0] == window) {
			return;
		}

		// Save the first element to the carry.
		carry = ui->ig_context->WindowsFocusOrder.Data[0];

		// Replace the first element with our window.
		ui->ig_context->WindowsFocusOrder.Data[0] = window;

		// Nodge all of the remainder elements to the right, stopping if we find our previous selves again.
		for (int i = 1; i < ui->ig_context->WindowsFocusOrder.Size; i++) {
			// Fetch the current element.
			ImGuiWindow *element = ui->ig_context->WindowsFocusOrder.Data[i];

			// Replace the element by the carry.
			ui->ig_context->WindowsFocusOrder.Data[i] = carry;

			// The element becomes the new carry.
			carry = element;

			// At this point, if we meet the original window, we can short-circuit the algorithm.
			if (element == window) {
				break;
			}
		}
	}
}

static void render_settings(struct ui *ui) {
	center_next_window();

	igSetNextWindowSize((ImVec2) { 300, 0}, ImGuiCond_Once);

	if (igBegin("Settings", &ui->show_settings, ImGuiWindowFlags_NoResize)) {
		char buffer[1024];
		snprintf(buffer, sizeof buffer, "%s", client.window.title);
		if (igInputText("Window title", buffer, sizeof buffer, ImGuiInputTextFlags_None, NULL, NULL)) {
			window_update_title(&client.window, buffer);
		}

		if (igSliderFloat("FOV", &client.renderer.fov, 10.0f, 120.0f, "%.0f", ImGuiSliderFlags_None)) {
			renderer_update_projection_matrix(&client.renderer);
		}
	}

	igEnd();
}

static void render_debug_camera(struct ui *ui) {
	igSetNextWindowSize((ImVec2) { 300, 210}, ImGuiCond_Once);

	if (igBegin("Debug camera", &ui->show_debug_camera, ImGuiWindowFlags_NoResize)) {
		igText("Eye");

		igSetNextItemWidth(-60);

		igPushStyleColorVec4(ImGuiCol_FrameBg, (ImVec4) { 0.140, 0.140, 0.140, 1});
		igPushStyleColorVec4(ImGuiCol_FrameBgActive, (ImVec4) { 0.140, 0.140, 0.140, 1});
		igPushStyleColorVec4(ImGuiCol_FrameBgHovered, (ImVec4) { 0.140, 0.140, 0.140, 1});

		if (igDragFloat3("Position##camera_eye", client.camera.eye, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_ReadOnly)) {
			camera_update(&client.camera);
		}

		igPopStyleColor(3);

		igSetNextItemWidth(-60);

		if (igDragFloat("Distance##eye_distance", &client.camera.eye_distance, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
			camera_update(&client.camera);
		}

		igSetNextItemWidth(-60);

		if (igDragFloat("Around##eye_around", &client.camera.eye_around, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
			camera_update(&client.camera);
		}

		igSetNextItemWidth(-60);

		if (igDragFloat("Above##eye_above", &client.camera.eye_above, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
			camera_update(&client.camera);
		}

		igSeparator();

		igText("Center");

		igSetNextItemWidth(-60);

		if (igDragFloat3("Position##camera_center", client.camera.center, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
			camera_update(&client.camera);
		}

		igSetNextItemWidth(-60);

		if (igDragFloat("Rotation##center_rotation", &client.camera.center_rotation, 0.01, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None)) {
			camera_update(&client.camera);
		}
	}

	igEnd();
}

static void render_debug_tools(struct ui *ui) {
	center_next_window();

	igSetNextWindowSize((ImVec2) { 380, 0}, ImGuiCond_Once);

	if (igBegin("Debugging", &ui->show_debug_tools, ImGuiWindowFlags_NoResize)) {
		igText("Cursor position: %.0f %.0f", client.window.cursor_pos_x, client.window.cursor_pos_y);
		igText("Mouse picked: %u", client.renderer.mousepicking_entity_id);
		igText("Selected entity: %u", ui->selected_entity_id);

		igSeparator();

		if (igCheckbox("Wireframe mode", &client.renderer.wireframe)) {
			renderer_wireframe(&client.renderer, client.renderer.wireframe);
		}

		if (igCheckbox("Fullscreen", &client.window.fullscreen)) {
			window_fullscreen(&client.window, client.window.fullscreen);
		}

		igCheckbox("Debug camera", &ui->show_debug_camera);
		igCheckbox("ImGUI Demo Window", &ui->show_imgui_demo);

		igSeparator();

		igSetNextItemWidth(-130);

		const char *choices[] = {"None", "Translation", "Rotation", "Scale"};
		static int mode = 0;
		if (igComboStr_arr("Gizmo mode", &mode, choices, ARRAY_COUNT(choices), 5)) {
			switch (mode) {
			    case 1: ui->gizmo.mode = GIZMO_MODE_TRANSLATION; break;
			    case 2: ui->gizmo.mode = GIZMO_MODE_ROTATION; break;
			    case 3: ui->gizmo.mode = GIZMO_MODE_SCALE; break;
			    default: ui->gizmo.mode = GIZMO_MODE_NONE;
			}
		}

		igText("Moving bitmask: %d", client.moving);
	}

	igEnd();
}

static void render_about(struct ui *ui) {
	center_next_window();

	// The window.
	if (igBegin(DEFAULT_TITLE, &ui->show_about, ImGuiWindowFlags_NoResize)) {
		float width = igGetWindowWidth();

		// Display the logo centered.
		igSetCursorPosX((width - ui->assets_logo_png.width / 2) / 2);
		ImTextureID texture_id = (void *) (uintptr_t) ui->assets_logo_png.gl_id; // Absolutely gross.
		igImage(texture_id, (ImVec2) { ui->assets_logo_png.width / 2, ui->assets_logo_png.height / 2}, (ImVec2) { 0, 0}, (ImVec2) { 1, 1}, (ImVec4) { 1, 1, 1, 1}, (ImVec4) { 0, 0, 0, 0});

		igSeparator();

		igText("Alex Belanger (nitrix)");
		igText("https://github.com/nitrix/layman");

		igSeparator();

		// Footer.
		char buffer[256];
		sprintf(buffer, "Version %s", VERSION);
		prepare_centered_text(buffer);
		igTextDisabled(buffer);
	}

	igEnd();
}

static void render_scene_editor(struct ui *ui) {
	igSetNextWindowSize((ImVec2) { 350, 340}, ImGuiCond_Once);

	center_next_window();

	if (igBegin("Scene editor", &ui->show_scene_editor, ImGuiWindowFlags_NoResize)) {
		static char buf[1024] = "assets/DamagedHelmet.glb";
		igSetNextItemWidth(-70);
		igInputText("##scene-load", buf, sizeof buf, ImGuiInputTextFlags_None, NULL, NULL);
		igSameLine(0, -1);
		igSetNextItemWidth(70);
		if (igButton("Load", (ImVec2) { -1, 0})) {
			struct entity *entity = malloc(sizeof *entity);
			if (entity_init(entity, buf)) {
				scene_add_entity(&client.scene, entity);
				buf[0] = '\0';
			}
		}

		igSeparator();

		igPushStyleColorVec4(ImGuiCol_ChildBg, (ImVec4) { 0.16, 0.29, 0.48, 0.54});

		igBeginChildStr("##scene-entities", (ImVec2) { -1, 200}, false, ImGuiWindowFlags_None);

		struct entity *found_entity = NULL;

		for (size_t i = 0; i < client.scene.entity_count; i++) {
			struct entity *entity = client.scene.entities[i];

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;

			if (entity->id == ui->selected_entity_id) {
				flags |= ImGuiTreeNodeFlags_Selected;
				found_entity = entity;
			}

			char buffer[1024];
			snprintf(buffer, sizeof buffer, "##entity-index-%zu", i);

			if (igTreeNodeExStrStr(buffer, flags, "%s", entity->model->filepath)) {
				igTreePop();
			}

			if (igIsItemClicked(ImGuiMouseButton_Left)) {
				ui->selected_entity_id = entity->id;
				found_entity = entity;
			}
		}

		igEndChild();

		igPopStyleColor(1);

		igSeparator();

		if (found_entity) {
			if (igButton("R##reset-translation", (ImVec2) { 0, 0})) {
				glm_vec3_zero(found_entity->translation);
			}

			igSameLine(0, -1);
			igDragFloat3("Translation", found_entity->translation, step_size, -FLT_MAX, FLT_MAX, "%f", ImGuiSliderFlags_None);

			if (igButton("R##reset-rotation", (ImVec2) { 0, 0})) {
				glm_quat_identity(found_entity->rotation);
			}

			igSameLine(0, -1);

			vec3 old_euler_rotation, new_euler_rotation;
			mat4 rotation;

			glm_quat_mat4(found_entity->rotation, rotation);
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

				glm_quat_mul(found_entity->rotation, r, found_entity->rotation);
			}

			if (igButton("R##reset-scale", (ImVec2) { 0, 0})) {
				found_entity->scale = 1;
			}

			igSameLine(0, -1);
			igDragFloat("Scale", &found_entity->scale, step_size, 0, FLT_MAX, "%f", ImGuiSliderFlags_None);
		} else {
			igText("Select an entity.");
		}
	}

	igEnd();
}

static void render_menu_entry(GLuint texture_id, bool *active) {
	if (*active) {
		igPushStyleColorVec4(ImGuiCol_Button, (ImVec4) { 0.26, 0.59, 0.98, 0.6});
	} else {
		igPushStyleColorVec4(ImGuiCol_Button, (ImVec4) { 0.06, 0.53, 0.98, 0.2});
	}

	if (igImageButton((void *) (uintptr_t) texture_id, (ImVec2) { 32, 32}, (ImVec2) { 0, 0}, (ImVec2) { 1, 1}, 10, (ImVec4) { 0, 0, 0, 0}, (ImVec4) { 1, 1, 1, 1})) {
		*active = !*active;
	}

	igPopStyleColor(1);
}

static void render_menu(struct ui *ui) {
	igSetNextWindowPos((ImVec2) { 10, 10}, ImGuiCond_Always, (ImVec2) { 0, 0});

	if (igBegin("Menu", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		// Force our menu to be behind everything.
		igBringWindowToDisplayBack(igGetCurrentWindow());

		// Force our menu window to have the least focus.
		bring_window_to_focus_back(ui, igGetCurrentWindow());

		render_menu_entry(ui->assets_cube_png.gl_id, &ui->show_scene_editor);
		render_menu_entry(ui->assets_bug_png.gl_id, &ui->show_debug_tools);
		render_menu_entry(ui->assets_gear_png.gl_id, &ui->show_settings);
		render_menu_entry(ui->assets_question_png.gl_id, &ui->show_about);
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (ui->show) {
		render_menu(ui);

		if (ui->show_debug_tools) {
			render_debug_tools(ui);
		}

		if (ui->show_scene_editor) {
			render_scene_editor(ui);
		}

		if (ui->show_settings) {
			render_settings(ui);
		}

		if (ui->show_debug_camera) {
			render_debug_camera(ui);
		}

		if (ui->show_about) {
			render_about(ui);
		}

		if (ui->show_imgui_demo) {
			igShowDemoWindow(&ui->show_imgui_demo);
		}

		gizmo_render(&client.ui.gizmo);
	}

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

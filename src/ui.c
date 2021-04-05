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
	ui->show_debugging_tools = false;
	ui->show_settings = false;
	ui->show_about = false;
	ui->show_test = false;

	ui->selected_entity_id = 0;

	ImGui_ImplGlfw_InitForOpenGL(client.window.glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");
	igStyleColorsDark(NULL);

	texture_init_from_memory(&ui->assets_logo_png, TEXTURE_KIND_IMAGE, assets_logo_png_data, assets_logo_png_size);
	texture_init_from_memory(&ui->assets_gear_png, TEXTURE_KIND_IMAGE, assets_gear_png_data, assets_gear_png_size);
	texture_init_from_memory(&ui->assets_bug_png, TEXTURE_KIND_IMAGE, assets_bug_png_data, assets_bug_png_size);
	texture_init_from_memory(&ui->assets_question_png, TEXTURE_KIND_IMAGE, assets_question_png_data, assets_question_png_size);
	texture_init_from_memory(&ui->assets_cube_png, TEXTURE_KIND_IMAGE, assets_cube_png_data, assets_cube_png_size);

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
}

static float closest_distance_between_two_rays(vec3 r1_origin, vec3 r1_direction, vec3 r2_origin, vec3 r2_direction, float *d1, float *d2) {
	vec3 dp;
	glm_vec3_sub(r2_origin, r1_origin, dp);

	float v12 = glm_vec3_dot(r1_direction, r1_direction);
	float v22 = glm_vec3_dot(r2_direction, r2_direction);
	float v1v2 = glm_vec3_dot(r1_direction, r2_direction);

	float det = v1v2 * v1v2 - v12 * v22;

	if (fabs(det) > FLT_MIN) {
		float inv_det = 1.f / det;

		float dpv1 = glm_vec3_dot(dp, r1_direction);
		float dpv2 = glm_vec3_dot(dp, r2_direction);

		// FIXME: I had to invert their signs, not sure why.
		float t1 = -1 * inv_det * (v22 * dpv1 - v1v2 * dpv2);
		float t2 = -1 * inv_det * (v1v2 * dpv1 - v12 * dpv2);

		*d1 = t1;
		*d2 = t2;

		glm_vec3_muladds(r2_direction, t2, dp);
		glm_vec3_muladds(r1_direction, -t1, dp);
		return glm_vec3_norm(dp);
	} else {
		vec3 a;
		glm_vec3_cross(dp, r1_direction, a);
		return sqrt(glm_vec3_dot(a, a) / v12);
	}
}

static void render_test(struct ui *ui) {
	float t1, t2;

	vec3 r1_origin, r1_direction, r2_origin, r2_direction;

	glm_vec4_copy3(client.window.cursor_ray_origin, r1_origin);
	glm_vec4_copy3(client.window.cursor_ray_direction, r1_direction);

	// HACK: Very hacky. Remove.
	if (client.scene.entity_count >= 1) {
		glm_vec3_copy(client.scene.entities[0]->translation, r2_origin);
	}

	/*
	   r2_origin[0] = 0;
	   r2_origin[1] = 0;
	   r2_origin[2] = -10;
	 */

	r2_direction[0] = 0;
	r2_direction[1] = 1;
	r2_direction[2] = 0;

	float distance = closest_distance_between_two_rays(r1_origin, r1_direction, r2_origin, r2_direction, &t1, &t2);

	if (igBegin("Test", &ui->show_test, ImGuiWindowFlags_None)) {
		igText("R1 Orig: %f %f %f", r1_origin[0], r1_origin[1], r1_origin[2]);
		igText("R1 Dir: %f %f %f", r1_direction[0], r1_direction[1], r1_direction[2]);
		igText("R2 Orig: %f %f %f", r2_origin[0], r2_origin[1], r2_origin[2]);
		igText("R2 Dir: %f %f %f", r2_direction[0], r2_direction[1], r2_direction[2]);

		igText("Distance: %f", distance);
		igText("T1: %f", t1);
		igText("T2: %f", t2);

		static bool move;
		igCheckbox("Move selected entity", &move);

		if (move) {
			for (size_t i = 0; i < client.scene.entity_count; i++) {
				struct entity *entity = client.scene.entities[i];
				if (entity->id == ui->selected_entity_id) {
					glm_vec3_copy(r2_origin, entity->translation);
					glm_vec3_muladds(r2_direction, t2, entity->translation);
				}
			}
		}
	}

	igEnd();
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

static void render_settings(struct ui *ui) {
	center_next_window();

	igSetNextWindowSize((ImVec2) { 300, 0}, ImGuiCond_Once);

	if (igBegin("Settings", &ui->show_settings, ImGuiWindowFlags_None)) {
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

static void render_debugging_tools(struct ui *ui) {
	center_next_window();

	igSetNextWindowSize((ImVec2) { 200, 0}, ImGuiCond_Once);

	if (igBegin("Debugging tools", &ui->show_debugging_tools, ImGuiWindowFlags_None)) {
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

		igCheckbox("ImGUI Demo Window", &ui->show_imgui_demo);

		#if DEBUG
		igCheckbox("Test window", &ui->show_test);
		#endif
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

	if (igBegin("Scene editor", &ui->show_scene_editor, ImGuiWindowFlags_None)) {
		static char buf[1024] = "assets/foo.glb";
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

static void render_menu_bar_entry(GLuint texture_id, bool *active) {
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

static void render_menu_bar(struct ui *ui) {
	igSetNextWindowPos((ImVec2) { 10, 10}, ImGuiCond_Always, (ImVec2) { 0, 0});

	if (igBegin("Menu", NULL, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)) {
		render_menu_bar_entry(ui->assets_cube_png.gl_id, &ui->show_scene_editor);
		render_menu_bar_entry(ui->assets_bug_png.gl_id, &ui->show_debugging_tools);
		render_menu_bar_entry(ui->assets_gear_png.gl_id, &ui->show_settings);
		render_menu_bar_entry(ui->assets_question_png.gl_id, &ui->show_about);
	}

	igEnd();
}

void ui_render(struct ui *ui) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (ui->show) {
		render_menu_bar(ui);

		if (ui->show_debugging_tools) {
			render_debugging_tools(ui);
		}

		if (ui->show_scene_editor) {
			render_scene_editor(ui);
		}

		if (ui->show_settings) {
			render_settings(ui);
		}

		if (ui->show_about) {
			render_about(ui);
		}

		if (ui->show_imgui_demo) {
			igShowDemoWindow(&ui->show_imgui_demo);
		}

		if (ui->show_test) {
			render_test(ui);
		}
	}

	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

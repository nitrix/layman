#include "cimgui.h"
#include "client.h"

void gizmo_init(struct gizmo *gizmo) {
	gizmo->mode = GIZMO_MODE_NONE;
}

void gizmo_fini(struct gizmo *gizmo) {
	UNUSED(gizmo);
}

static void gizmo_render_translation(struct gizmo *gizmo) {
	UNUSED(gizmo);

	static bool dragging = false;
	static size_t drag_axis = 0;
	static vec3 drag_start;

	struct entity *entity = find_selected_entity();

	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_translate(model_matrix, entity->translation);

	bool proximity_already = false;

	for (size_t axis = 0; axis < 3; axis++) {
		float t1, t2;

		vec3 r1_origin, r1_direction, r2_origin, r2_direction;

		glm_vec4_copy3(client.window.cursor_ray_origin, r1_origin);
		glm_vec4_copy3(client.window.cursor_ray_direction, r1_direction);

		glm_vec3_copy(entity->translation, r2_origin);

		glm_vec3_zero(r2_direction);
		r2_direction[axis] = 1;

		float distance = utils_closest_distance_between_two_rays(r1_origin, r1_direction, r2_origin, r2_direction, &t1, &t2);

		vec3 line_start = GLM_VEC3_ZERO_INIT;
		vec3 line_end;
		glm_vec3_copy(r2_direction, line_end); // The line end, happens to be the same as the ray direction.
		vec4 color;
		glm_vec3_copy(r2_direction, color); // The color of the line, happens to be the same as the ray direction.

		// Closest axis intersect
		vec3 intersect;
		glm_vec3_copy(r2_origin, intersect);
		glm_vec3_muladds(r2_direction, t2, intersect);

		// Proximity.
		bool on_axis = distance < 0.05 && t2 >= 0 && t2 <= 1;
		bool proximity = on_axis && !dragging && !client.ui.ig_io->WantCaptureMouse && !proximity_already;

		if (proximity) {
			proximity_already = true;
		}

		// Mouse dragging.
		if (proximity && igIsMouseDragging(ImGuiMouseButton_Left, 0)) {
			if (!dragging) {
				dragging = true;
				drag_axis = axis;
				glm_vec3_copy(intersect, drag_start);
			}
		}

		if (igIsMouseReleased(ImGuiMouseButton_Left)) {
			dragging = false;
		}

		bool dragging_current_axis = dragging && axis == drag_axis;

		// Coloring the axis yellow.
		if (proximity || dragging_current_axis) {
			glm_vec4_copy((vec4) { 1, 1, 0, 1}, color); // Yellow.
		}

		// The actual mutation by the delta.
		if (dragging_current_axis) {
			vec3 delta;
			glm_vec3_sub(intersect, drag_start, delta);
			glm_vec3_add(entity->translation, delta, entity->translation);
			glm_vec3_copy(intersect, drag_start);

			// Massive guide line.
			vec3 guide_line_start, guide_line_end;
			glm_vec3_zero(guide_line_start);
			glm_vec3_zero(guide_line_end);
			glm_vec3_muladds(r2_direction, 1000, guide_line_end);
			utils_render_line(guide_line_start, guide_line_end, model_matrix, (vec4) { 0.5, 0.5, 0.5, 1});
			glm_vec3_zero(guide_line_end);
			glm_vec3_muladds(r2_direction, -1000, guide_line_end);
			utils_render_line(guide_line_start, guide_line_end, model_matrix, (vec4) { 0.5, 0.5, 0.5, 1});
		}

		// Render axis handle.
		utils_render_line(line_start, line_end, model_matrix, color);
	}

	if (dragging) {
		client.ui.ig_io->WantCaptureMouse = true;
	}
}

static void gizmo_render_rotation(struct gizmo *gizmo) {
	UNUSED(gizmo);

	struct entity *entity = find_selected_entity();

	for (size_t axis = 0; axis < 3; axis++) {
		vec3 circle_origin = {0};
		glm_vec3_copy(entity->translation, circle_origin);

		vec3 circle_orientation = {0};

		circle_orientation[axis] = 1;

		float circle_radius = 1;

		vec3 closest = {0};

		mat4 transform = GLM_MAT4_IDENTITY_INIT;

		glm_translate(transform, entity->translation);
		glm_quat_rotate(transform, entity->rotation, transform);
		switch (axis) {
		    case 0:
			    glm_rotate_y(transform, circle_orientation[0] * M_PI_2, transform);
			    break;
		    case 1: glm_rotate_x(transform, circle_orientation[1] * M_PI_2, transform); break;
		    case 2: glm_rotate_z(transform, circle_orientation[2] * M_PI_2, transform); break;
		}

		vec4 color = {circle_orientation[0], circle_orientation[1], circle_orientation[2], 1};

		mat4 t = GLM_MAT4_IDENTITY_INIT;
		glm_quat_rotate(t, entity->rotation, t);
		glm_vec3_rotate_m4(t, circle_orientation, circle_orientation);

		float distance = utils_closest_distance_between_ray_and_circle(
				client.window.cursor_ray_origin,
				client.window.cursor_ray_direction,
				circle_origin,
				circle_orientation,
				circle_radius,
				closest);

		bool proximity = distance < 0.1 && distance >= 0;

		if (proximity) {
			glm_vec4_copy((vec4) { 1, 1, 0, 1}, color);
		}

		utils_render_ngon(64, circle_radius, transform, color);
	}
}

void gizmo_render(struct gizmo *gizmo) {
	// Don't draw any gizmo if no entity is selected.
	if (!client.ui.selected_entity_id) {
		return;
	}

	// Don't pay attention to the depth buffer.
	glDisable(GL_DEPTH_TEST);

	// Don't mess up the depth buffer either.
	glDepthMask(false);

	switch (gizmo->mode) {
	    case GIZMO_MODE_TRANSLATION: gizmo_render_translation(gizmo); return;
	    case GIZMO_MODE_ROTATION: gizmo_render_rotation(gizmo); return;
	    default: return;
	}
}

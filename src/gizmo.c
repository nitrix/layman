#include "client.h"

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

void gizmo_init(struct gizmo *gizmo) {
	gizmo->arrow_model = model_load("assets/arrow.glb");
	if (!gizmo->arrow_model) {
		fprintf(stderr, "Unable to load arrow model\n");
		return;
	}

	gizmo->mode = GIZMO_MODE_NONE;
}

void gizmo_fini(struct gizmo *gizmo) {
	model_destroy(gizmo->arrow_model);
}

static void gizmo_render_translation(struct gizmo *gizmo) {
	struct mesh *mesh = &gizmo->arrow_model->meshes[0];
	struct entity *entity = find_selected_entity();

	mesh_switch(mesh);
	glUseProgram(mesh->shader->program_id);

	// FIXME?
	mesh->material.roughness_factor = 1;
	mesh->material.normal_scale = 1;

	mat4 view_projection_matrix;
	glm_mat4_mul(client.renderer.projection_matrix, (vec4 *) client.camera.view_matrix, view_projection_matrix);

	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_mat4_copy(mesh->initial_transform, model_matrix);

	// Translation, rotation, scale.
	// glm_translate(model_matrix, (vec3) { 0, 0, -5});
	// glm_quat_rotate(model_matrix, (float *) entity->rotation, model_matrix);
	// glm_scale(model_matrix, (vec3) { entity->scale, entity->scale, entity->scale});

	// Uniforms.
	shader_bind_uniform_camera(mesh->shader, &client.camera);

	// Render.
	// FIXME: Support more than just unsigned shorts.

	mat4 tmp_matrix;

	// X
	glm_mat4_identity(tmp_matrix);
	glm_translate(tmp_matrix, entity->translation);
	glm_rotate_z(tmp_matrix, -M_PI_2, tmp_matrix);
	glm_mat4_mul(tmp_matrix, model_matrix, tmp_matrix);
	glm_vec4_copy((vec4) { 1, 0, 0, 1}, mesh->material.base_color_factor);
	shader_bind_uniform_mvp(mesh->shader, view_projection_matrix, tmp_matrix, 1);
	shader_bind_uniform_material(mesh->shader, &mesh->material);
	glDrawElements(GL_TRIANGLES, mesh->indices_count, mesh->indices_type, NULL);

	// Y
	glm_mat4_identity(tmp_matrix);
	glm_translate(tmp_matrix, entity->translation);
	glm_mat4_mul(tmp_matrix, model_matrix, tmp_matrix);
	glm_vec4_copy((vec4) { 0, 1, 0, 1}, mesh->material.base_color_factor);
	shader_bind_uniform_mvp(mesh->shader, view_projection_matrix, tmp_matrix, 1);
	shader_bind_uniform_material(mesh->shader, &mesh->material);
	glDrawElements(GL_TRIANGLES, mesh->indices_count, mesh->indices_type, NULL);

	// Z
	glm_mat4_identity(tmp_matrix);
	glm_translate(tmp_matrix, entity->translation);
	glm_rotate_x(tmp_matrix, M_PI_2, tmp_matrix);
	glm_mat4_mul(tmp_matrix, model_matrix, tmp_matrix);
	glm_vec4_copy((vec4) { 0, 0, 1, 1}, mesh->material.base_color_factor);
	shader_bind_uniform_mvp(mesh->shader, view_projection_matrix, tmp_matrix, 1);
	shader_bind_uniform_material(mesh->shader, &mesh->material);
	glDrawElements(GL_TRIANGLES, mesh->indices_count, mesh->indices_type, NULL);

	/*
	   float t1, t2;

	   vec3 r1_origin, r1_direction, r2_origin, r2_direction;

	   glm_vec4_copy3(client.window.cursor_ray_origin, r1_origin);
	   glm_vec4_copy3(client.window.cursor_ray_direction, r1_direction);

	   // HACK: Very hacky. Remove.
	   if (client.scene.entity_count >= 1) {
	        glm_vec3_copy(client.scene.entities[0]->translation, r2_origin);
	   }

	   // r2_origin[0] = 0;
	   // r2_origin[1] = 0;
	   // r2_origin[2] = -10;

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
	 */
}

void gizmo_render(struct gizmo *gizmo) {
	// Don't draw any gizmo if no entity is selected.
	if (!client.ui.selected_entity_id) {
		return;
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	switch (gizmo->mode) {
	    case GIZMO_MODE_TRANSLATION: gizmo_render_translation(gizmo); return;
	    default: return;
	}
}

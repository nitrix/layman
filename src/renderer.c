#include "layman.h"

thread_local const struct layman_renderer *current_renderer;

struct layman_renderer *layman_renderer_create(void) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	// TODO: Dynamic dimensions.
	renderer->viewport_width = 1280;
	renderer->viewport_height = 720;

	// TODO: Change the FOV.
	renderer->fov = 90.0f;
	renderer->far_plane = 1000.0f;
	renderer->near_plane = 0.1f;

	renderer->start_time = glfwGetTime();
	renderer->viewProjectionMatrixLocation = -1;
	renderer->modelMatrixLocation = -1;
	renderer->normalMatrixLocation = -1;

	renderer->exposure = 1;

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	if (!renderer) {
		return;
	}

	free(renderer);
}

void layman_renderer_switch(const struct layman_renderer *renderer) {
	if (current_renderer == renderer) {
		return;
	} else {
		current_renderer = renderer;
	}

	// Resize the viewport, go back to the default framebuffer and clear color for rendering.
	glViewport(0, 0, renderer->viewport_width, renderer->viewport_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1); // Black.

	// TODO: Support a wireframe mode.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Multisampling.
	glEnable(GL_MULTISAMPLE);

	// Necessary to avoid the seams of the cubemap being visible.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void calculate_projection_matrix(const struct layman_renderer *renderer, mat4 out) {
	float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
	float scale_y = (1.0f / tanf(renderer->fov / 2.0f / 180.0f * M_PI)) * aspect_ratio;
	float scale_x = scale_y / aspect_ratio;
	float frustrum_length = renderer->near_plane - renderer->far_plane;

	out[0][0] = scale_x;
	out[1][1] = scale_y;
	out[2][2] = ((renderer->far_plane + renderer->near_plane) / frustrum_length);
	out[3][2] = -1;
	out[2][3] = ((2 * renderer->near_plane * renderer->far_plane) / frustrum_length);
}

static void render_mesh(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene, const struct layman_mesh *mesh) {
	layman_mesh_switch(mesh);

	// Uniforms.
	layman_shader_bind_uniform_material(mesh->shader, mesh->material);
	layman_shader_bind_uniform_camera(mesh->shader, camera);
	layman_shader_bind_uniform_lights(mesh->shader, scene->lights, scene->lights_count);

	// FIXME: Horrible, please don't do this every frames!
	if (renderer->viewProjectionMatrixLocation == -1) {
		renderer->viewProjectionMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ViewProjectionMatrix");
		renderer->modelMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ModelMatrix");
		renderer->normalMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_NormalMatrix");
		renderer->exposureLocation = glGetUniformLocation(mesh->shader->program_id, "u_Exposure");
	}

	// TODO: More uniforms, tidy this up.
	// TODO: Should all move into the model file and stuff.
	double elapsed = layman_renderer_elapsed(renderer);

	mat4 view_matrix, projection_matrix, view_projection_matrix;
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, -5}, (vec3) { 0, 1, 0}, view_matrix); // FIXME: Static camera for now.
	glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection_matrix);
	// glm_mat4_mul(projection_matrix, view_matrix, view_projection_matrix);
	glm_mat4_mul(view_matrix, projection_matrix, view_projection_matrix);
	glUniformMatrix4fv(renderer->viewProjectionMatrixLocation, 1, false, view_projection_matrix[0]);

	// Translation, rotation (z, y, x), scale.
	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_translate_z(model_matrix, -3);
	glm_rotate_y(model_matrix, M_PI * elapsed * 0.25f, model_matrix);
	glm_rotate_x(model_matrix, M_PI_2, model_matrix);
	// glm_mat4_scale(model_matrix, 1);

	glUniformMatrix4fv(renderer->modelMatrixLocation, 1, false, model_matrix[0]);
	glUniformMatrix4fv(renderer->normalMatrixLocation, 1, false, model_matrix[0]);
	glUniform1f(renderer->exposureLocation, renderer->exposure);

	// Render.
	// FIXME: Support more than just unsigned shorts.
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
}

double layman_renderer_elapsed(const struct layman_renderer *renderer) {
	return glfwGetTime() - renderer->start_time;
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	layman_renderer_switch(renderer);

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all entities.
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct layman_entity *entity = scene->entities[i];

		// Render all meshes.
		for (size_t i = 0; i < entity->model->meshes_count; i++) {
			struct layman_mesh *mesh = entity->model->meshes[i];

			// Render mesh.
			render_mesh(renderer, camera, scene, mesh);
		}
	}
}

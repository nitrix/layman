#include "layman.h"

struct layman_renderer *layman_renderer_create(const struct layman_window *window) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	// TODO: Dynamic dimensions.
	renderer->viewport_width = window->width;
	renderer->viewport_height = window->height;

	// TODO: Change the FOV.
	renderer->fov = 90.0f;
	renderer->far_plane = 1000.0f;
	renderer->near_plane = 0.1f;

	renderer->start_time = glfwGetTime();
	renderer->exposure = 1;

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	if (!renderer) {
		return;
	}

	free(renderer);
}

void layman_renderer_switch(const struct layman_renderer *new) {
	thread_local static const struct layman_renderer *current;

	if (current == new) {
		return;
	}

	current = new;

	if (!new) {
		return;
	}

	// Resize the viewport, go back to the default framebuffer and clear color for rendering.
	glViewport(0, 0, new->viewport_width, new->viewport_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1); // Black.

	// TODO: Support a wireframe mode.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// FIXME: Back face culling.
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);

	// Depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Multisampling.
	glEnable(GL_MULTISAMPLE);

	// Necessary to avoid the seams of the cubemap being visible.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

static void render_mesh(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene, const struct layman_mesh *mesh) {
	static GLint viewProjectionMatrixLocation = -1;
	static GLint modelMatrixLocation = -1;
	static GLint normalMatrixLocation = -1;
	static GLint exposureLocation = -1;

	layman_mesh_switch(mesh);

	// Uniforms.
	layman_shader_bind_uniform_environment(mesh->shader, scene->environment);
	layman_shader_bind_uniform_material(mesh->shader, mesh->material);
	layman_shader_bind_uniform_camera(mesh->shader, camera);
	layman_shader_bind_uniform_lights(mesh->shader, scene->lights, scene->lights_count);

	// FIXME: Horrible, please don't do this every frames!
	if (viewProjectionMatrixLocation == -1) {
		viewProjectionMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ViewProjectionMatrix");
		modelMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ModelMatrix");
		normalMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_NormalMatrix");
		exposureLocation = glGetUniformLocation(mesh->shader->program_id, "u_Exposure");
	}

	// TODO: More uniforms, tidy this up.
	// TODO: Should all move into the model file and stuff.
	double elapsed = layman_renderer_elapsed(renderer);

	mat4 view_matrix, projection_matrix, view_projection_matrix;
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, -5}, (vec3) { 0, 1, 0}, view_matrix); // FIXME: Static camera for now.
	glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection_matrix);
	// glm_mat4_mul(projection_matrix, view_matrix, view_projection_matrix);
	glm_mat4_mul(view_matrix, projection_matrix, view_projection_matrix);
	glUniformMatrix4fv(viewProjectionMatrixLocation, 1, false, view_projection_matrix[0]);

	// Translation, rotation (z, y, x), scale.
	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_translate_z(model_matrix, -3);
	glm_rotate_y(model_matrix, M_PI * elapsed * 0.25f, model_matrix);
	glm_rotate_x(model_matrix, M_PI_2, model_matrix);

	glUniformMatrix4fv(modelMatrixLocation, 1, false, model_matrix[0]);
	glUniformMatrix4fv(normalMatrixLocation, 1, false, model_matrix[0]);
	glUniform1f(exposureLocation, renderer->exposure);

	// Render.
	// FIXME: Support more than just unsigned shorts.
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
}

static void render_skybox(const struct layman_renderer *renderer, const struct layman_scene *scene) {
	static struct layman_shader *skybox_shader = NULL;
	static GLint environment_map_location = 0;
	static GLint projection_location = 0;
	static GLint view_location = 0;

	// FIXME: This shouldn't be here.
	if (skybox_shader == NULL) {
		skybox_shader = layman_shader_load_from_files("shaders/skybox/main.vert", "shaders/skybox/main.frag", NULL);
		if (skybox_shader == NULL) {
			fprintf(stderr, "Unable to load skybox shader\n");
			exit(EXIT_FAILURE); // TODO: Handle failure more gracefully.
		}

		layman_shader_switch(skybox_shader);

		environment_map_location = glGetUniformLocation(skybox_shader->program_id, "environmentMap");
		projection_location = glGetUniformLocation(skybox_shader->program_id, "projection");
		view_location = glGetUniformLocation(skybox_shader->program_id, "view");
	}

	double elapsed = layman_renderer_elapsed(renderer);

	layman_shader_switch(skybox_shader);
	layman_texture_switch(scene->environment->cubemap);
	glUniform1i(environment_map_location, scene->environment->cubemap->kind);
	mat4 projection = GLM_MAT4_IDENTITY_INIT;
	glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection);
	glUniformMatrix4fv(projection_location, 1, false, projection[0]);
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, -1}, (vec3) { 0, 1, 0}, view);
	// glm_rotate_y(view, M_PI * elapsed * -0.10, view);
	glUniformMatrix4fv(view_location, 1, false, view[0]);

	glDisable(GL_CULL_FACE);
	renderCube();
	glEnable(GL_CULL_FACE);
	layman_mesh_switch(NULL);
}

double layman_renderer_elapsed(const struct layman_renderer *renderer) {
	return glfwGetTime() - renderer->start_time;
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	layman_renderer_switch(renderer);
	layman_environment_switch(scene->environment);

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

	// Render the skybox.
	// This is done last so that only the fragments that aren't hiding it gets computed.
	// The shader is written such that the depth buffer is always 1.0 (the furtest away).
	render_skybox(renderer, scene);
}

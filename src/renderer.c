#include "layman.h"

struct layman_renderer *layman_renderer_create(const struct layman_window *window) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	// TODO: Dynamic dimensions.
	int width, height;
	glfwGetFramebufferSize(window->glfw_window, &width, &height);
	renderer->viewport_width = width;
	renderer->viewport_height = height;

	// TODO: Change the FOV.
	renderer->fov = 45.0f; // Degrees.
	renderer->far_plane = 1000.0f;
	renderer->near_plane = 0.1f;

	renderer->start_time = glfwGetTime();
	renderer->exposure = 1;

	renderer->window = window;

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
	// glDisable(GL_CULL_FACE);

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

	double elapsed = layman_window_elapsed(renderer->window);

	mat4 view_matrix, projection_matrix, view_projection_matrix;
	glm_lookat(camera->translation, (vec3) { 0, 0, 0}, (vec3) { 0, 1, 0}, view_matrix);
	glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection_matrix);
	glm_perspective(glm_rad(renderer->fov), renderer->viewport_width / renderer->viewport_height, renderer->near_plane, renderer->far_plane, projection_matrix);

	// glm_rotate_z(view_matrix, camera->rotation[2], view_matrix);
	// glm_rotate_y(view_matrix, camera->rotation[1], view_matrix);
	// glm_rotate_x(view_matrix, camera->rotation[0], view_matrix);

	glm_mat4_mul(projection_matrix, view_matrix, view_projection_matrix);
	glUniformMatrix4fv(viewProjectionMatrixLocation, 1, false, view_projection_matrix[0]);

	// Translation, rotation (z, y, x), scale.
	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_rotate_y(model_matrix, M_PI_2, model_matrix);
	glm_rotate_x(model_matrix, M_PI_2, model_matrix); // FIXME Should come from the glTF transforms?
	// glm_scale(model_matrix, (vec3) { 100, 100, 100});

	glUniformMatrix4fv(modelMatrixLocation, 1, false, model_matrix[0]);
	glUniformMatrix4fv(normalMatrixLocation, 1, false, model_matrix[0]);
	glUniform1f(exposureLocation, renderer->exposure);

	// Render.
	// FIXME: Support more than just unsigned shorts.
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
}

static void render_skybox(const struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	static struct layman_shader *skybox_shader = NULL;
	static GLint environment_map_location = 0;
	static GLint projection_location = 0;
	static GLint view_location = 0;

	// FIXME: All this shouldn't be here.
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

	layman_shader_switch(skybox_shader);
	layman_texture_switch(scene->environment->cubemap);
	glUniform1i(environment_map_location, scene->environment->cubemap->kind);
	mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;
	// glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection);
	glm_perspective(glm_rad(renderer->fov), renderer->viewport_width / renderer->viewport_height, renderer->near_plane, renderer->far_plane, projection_matrix);
	glUniformMatrix4fv(projection_location, 1, false, projection_matrix[0]);
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	glm_lookat(camera->translation, (vec3) { 0, 0, -1}, (vec3) { 0, 1, 0}, view);
	glm_rotate_y(view, camera->rotation[1], view);
	glUniformMatrix4fv(view_location, 1, false, view[0]);

	// glDisable(GL_CULL_FACE);
	renderCube();
	// glEnable(GL_CULL_FACE);
	layman_mesh_switch(NULL);
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	layman_window_switch(renderer->window);
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
	render_skybox(renderer, camera, scene);
}

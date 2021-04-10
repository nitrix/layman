#include "cglm/cglm.h"
#include "client.h"
#include "entity.h"
#include "incbin.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "utils.h"

#define RENDERER_FOV 45.0f
#define RENDERER_PLANE_FAR 10000.0f
#define RENDERER_PLANE_NEAR 0.1f

INCBIN(shaders_skybox_main_vert, "../shaders/skybox/main.vert");
INCBIN(shaders_skybox_main_frag, "../shaders/skybox/main.frag");

INCBIN(shaders_plain_main_vert, "../shaders/plain/main.vert");
INCBIN(shaders_plain_main_frag, "../shaders/plain/main.frag");

void renderer_update_projection_matrix(struct renderer *renderer) {
	glm_mat4_identity(renderer->projection_matrix);
	// glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection);
	glm_perspective(glm_rad(renderer->fov), renderer->viewport_width / renderer->viewport_height, renderer->near_plane, renderer->far_plane, renderer->projection_matrix);
}

void renderer_init(struct renderer *renderer) {
	// TODO: Dynamic dimensions; what happens when the window gets resized?
	int width, height;
	window_framebuffer_size(&client.window, &width, &height);
	renderer->viewport_width = width;
	renderer->viewport_height = height;

	// TODO: Should provide means of modifying these.
	renderer->fov = RENDERER_FOV;
	renderer->far_plane = RENDERER_PLANE_FAR;
	renderer->near_plane = RENDERER_PLANE_NEAR;
	renderer_update_projection_matrix(renderer);

	renderer->exposure = 1;
	renderer->wireframe = false;

	renderer->plain_shader = shader_load_from_memory(NULL, shaders_plain_main_vert_data, shaders_plain_main_vert_size, shaders_plain_main_frag_data, shaders_plain_main_frag_size, NULL, 0);
	if (!renderer->plain_shader) {
		return;
	}

	renderer->mousepicking_entity_id = 0;
}

void renderer_fini(struct renderer *renderer) {
	shader_destroy(renderer->plain_shader);
}

void renderer_switch(const struct renderer *new) {
	// Resize the viewport, go back to the default framebuffer and clear color for rendering.
	glViewport(0, 0, new->viewport_width, new->viewport_height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1); // Black.

	// Depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Do update the depth buffer.
	glDepthMask(GL_TRUE);

	// Multisampling.
	glEnable(GL_MULTISAMPLE);

	// Wireframe.
	if (new->wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Necessary to avoid the seams of the cubemap being visible.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Transparency.
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void render_mesh(struct renderer *renderer, const struct camera *camera, const struct scene *scene, const struct entity *entity, const struct shader *shader, const struct mesh *mesh) {
	mesh_switch(mesh);

	mat4 view_projection_matrix;
	glm_mat4_mul(renderer->projection_matrix, (vec4 *) camera->view_matrix, view_projection_matrix);

	mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;

	// Translation, rotation, scale.
	glm_translate(model_matrix, (float *) entity->translation);
	glm_quat_rotate(model_matrix, (float *) entity->rotation, model_matrix);
	glm_scale(model_matrix, (vec3) { entity->scale, entity->scale, entity->scale});
	glm_mat4_mul(model_matrix, (vec4 *) mesh->initial_transform, model_matrix);

	// Uniforms.
	shader_bind_uniform_environment(shader, scene->environment);
	shader_bind_uniform_material(shader, &mesh->material);
	shader_bind_uniform_camera(shader, camera);
	shader_bind_uniform_lights(shader, scene->lights, scene->lights_count);
	shader_bind_uniform_mvp(shader, view_projection_matrix, model_matrix, renderer->exposure);

	// Render.
	glDrawElements(GL_TRIANGLES, mesh->indices_count, mesh->indices_type, NULL);
}

static void render_skybox(const struct renderer *renderer, const struct camera *camera, const struct scene *scene) {
	static struct shader *skybox_shader = NULL;
	static GLint environment_map_location = 0;
	static GLint projection_location = 0;
	static GLint view_location = 0;

	// Must disable back-face culling because we're inside the cube.
	glDisable(GL_CULL_FACE);

	// FIXME: All this shouldn't be here.
	if (skybox_shader == NULL) {
		skybox_shader = shader_load_from_memory(NULL, shaders_skybox_main_vert_data, shaders_skybox_main_vert_size, shaders_skybox_main_frag_data, shaders_skybox_main_frag_size, NULL, 0);
		if (skybox_shader == NULL) {
			fprintf(stderr, "Unable to load skybox shader\n");
			exit(EXIT_FAILURE); // TODO: Handle failure more gracefully.
		}

		glUseProgram(skybox_shader->program_id);

		environment_map_location = glGetUniformLocation(skybox_shader->program_id, "environmentMap");
		projection_location = glGetUniformLocation(skybox_shader->program_id, "projection");
		view_location = glGetUniformLocation(skybox_shader->program_id, "view");
	}

	glUseProgram(skybox_shader->program_id);
	texture_switch(&scene->environment->cubemap);
	glUniform1i(environment_map_location, scene->environment->cubemap.kind);
	mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;
	// glm_perspective_default(renderer->viewport_width / renderer->viewport_height, projection);
	glm_perspective(glm_rad(renderer->fov), renderer->viewport_width / renderer->viewport_height, renderer->near_plane, renderer->far_plane, projection_matrix);
	glUniformMatrix4fv(projection_location, 1, false, projection_matrix[0]);
	glUniformMatrix4fv(view_location, 1, false, camera->view_matrix[0]);

	// glDisable(GL_CULL_FACE);
	utils_render_cube();
	// glEnable(GL_CULL_FACE);
}

void renderer_render(struct renderer *renderer, const struct camera *camera, const struct scene *scene) {
	renderer_switch(renderer);
	environment_switch(scene->environment);

	// Clear the screen.
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Disable multisampling for color mouse-picking.
	glDisable(GL_MULTISAMPLE);

	// Render all entities, for mouse picking.
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct entity *entity = scene->entities[i];

		// Render all meshes.
		for (size_t i = 0; i < entity->model->meshes_count; i++) {
			struct mesh *mesh = &entity->model->meshes[i];

			// Render mesh.
			glUseProgram(renderer->plain_shader->program_id);
			static GLint picking_color_location = -1;
			if (picking_color_location == -1) {
				picking_color_location = glGetUniformLocation(renderer->plain_shader->program_id, "u_Color");
			}

			// Convert entity ID to color.
			vec4 color;
			entity_id_as_color(entity->id, color);
			glUniform4fv(picking_color_location, 1, color);

			render_mesh(renderer, camera, scene, entity, renderer->plain_shader, mesh);
		}
	}

	// Mouse picking; super-duper slow, the framebuffer is on the GPU.
	double mouseX = client.window.cursor_pos_x;
	double mouseY = client.window.cursor_pos_y;
	if (mouseX > 0 && mouseY > 0 && mouseX < renderer->viewport_width && mouseY < renderer->viewport_height) {
		int x = mouseX, y = mouseY;
		vec4 color;
		glReadPixels(x, renderer->viewport_height - y, 1, 1, GL_RGBA, GL_FLOAT, color);

		// Convert color back to entity ID.
		renderer->mousepicking_entity_id = entity_color_as_id(color);
	}

	// Start over.
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Re-enable multisampling.
	glEnable(GL_MULTISAMPLE);

	// Render all entities.
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct entity *entity = scene->entities[i];

		// Render all meshes.
		for (size_t i = 0; i < entity->model->meshes_count; i++) {
			struct mesh *mesh = &entity->model->meshes[i];

			// Render mesh.
			glUseProgram(mesh->shader->program_id);
			render_mesh(renderer, camera, scene, entity, mesh->shader, mesh);
		}
	}

	// Render the skybox.
	// This is done last so that only the fragments that aren't hiding it gets computed.
	// The shader is written such that the depth buffer is always 1.0 (the furtest away).
	render_skybox(renderer, camera, scene);
}

void renderer_wireframe(struct renderer *renderer, bool enabled) {
	renderer->wireframe = enabled;
}

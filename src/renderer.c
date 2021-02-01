#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "layman.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

struct layman_renderer {
	// Viewport.
	int viewport_width;
	int viewport_height;

	// Projection matrix.
	float fov;
	float far_plane;
	float near_plane;

	// TODO: Temporary.
	double start_time;
	GLint viewProjectionMatrixLocation;
	GLint modelMatrixLocation;
	GLint normalMatrixLocation;
	GLint exposureLocation;

	float exposure;
};

struct layman_matrix_4f calculate_projection_matrix(const struct layman_renderer *renderer) {
	float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
	float scale_y = (1.0f / tanf(renderer->fov / 2.0f / 180.0f * M_PI)) * aspect_ratio;
	float scale_x = scale_y / aspect_ratio;
	float frustrum_length = renderer->near_plane - renderer->far_plane;

	struct layman_matrix_4f m = {
		.d = {
			[0] = scale_x,
			[5] = scale_y,
			[10] = ((renderer->far_plane + renderer->near_plane) / frustrum_length),
			[11] = -1,
			[14] = ((2 * renderer->near_plane * renderer->far_plane) / frustrum_length)
		},
	};

	return m;
}

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
	free(renderer);
}

void layman_renderer_use(struct layman_renderer *renderer) {
	(void) renderer; // Unused.

	glClearColor(0, 0, 0, 1); // Black.

	// FIXME: Wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// glFrontFace(GL_CW);
	// glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE);
}

void layman_renderer_unuse(struct layman_renderer *renderer) {
	(void) renderer; // Unused.
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	(void) renderer; // Unused.

	double current_time = glfwGetTime();
	double elapsed = current_time - renderer->start_time;

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all entities.
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct layman_entity *entity = scene->entities[i];

		// Render all meshes.
		for (size_t i = 0; i < entity->model->meshes_count; i++) {
			struct layman_mesh *mesh = entity->model->meshes[i];

			// Render mesh.
			{
				glBindVertexArray(mesh->vao);
				layman_shader_use(mesh->shader);
				layman_material_use(mesh->material);

				// Uniforms.
				layman_shader_bind_uniform_material(mesh->shader, mesh->material);
				layman_shader_bind_uniform_camera(mesh->shader, camera);
				layman_shader_bind_uniform_lights(mesh->shader, scene->lights, scene->lights_count);

				// TODO: Horrible, please don't do this every frames!
				if (renderer->viewProjectionMatrixLocation == -1) {
					renderer->viewProjectionMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ViewProjectionMatrix");
					renderer->modelMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ModelMatrix");
					renderer->normalMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_NormalMatrix");
					renderer->exposureLocation = glGetUniformLocation(mesh->shader->program_id, "u_Exposure");
				}

				// TODO: More uniforms, tidy this up.
				// TODO: Should all move into the model file and stuff.
				struct layman_matrix_4f projectionMatrix = calculate_projection_matrix(renderer);
				glUniformMatrix4fv(renderer->viewProjectionMatrixLocation, 1, false, projectionMatrix.d); // TODO: Missing view matrix?
				struct layman_matrix_4f modelMatrix = layman_matrix_4f_identity();
				layman_matrix_4f_rotate_x(&modelMatrix, -M_PI_2);
				layman_matrix_4f_rotate_y(&modelMatrix, M_PI * elapsed * 0.25f);
				layman_matrix_4f_translate(&modelMatrix, LAYMAN_VECTOR_3F(0, 0, -3));
				glUniformMatrix4fv(renderer->modelMatrixLocation, 1, false, modelMatrix.d);
				// struct layman_matrix_4f normalMatrix = layman_matrix_4f_identity();
				// glUniformMatrix4fv(renderer->normalMatrixLocation, 1, false, normalMatrix.d);
				glUniformMatrix4fv(renderer->normalMatrixLocation, 1, false, modelMatrix.d); // OMG, this fixed the lighting problem!
				glUniform1f(renderer->exposureLocation, renderer->exposure);

				// Render.
				glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);

				layman_material_unuse(mesh->material);
				layman_shader_unuse(mesh->shader);
				glBindVertexArray(0);
			}
		}
	}
}

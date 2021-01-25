#include "layman.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

struct layman_renderer {
	struct layman_shader *pbr_shader;

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
	GLint sceneRotationMatrixLocation;
};

struct layman_matrix_4f calculate_projection_matrix(const struct layman_renderer *renderer) {
	float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
	float scale_y = (1.0f / tanf(renderer->fov)) * aspect_ratio;
	float scale_x = scale_y / aspect_ratio;
	float frustrum_length = renderer->near_plane - renderer->far_plane;

	struct layman_matrix_4f m = {
	    .d = {
	        [0] = scale_x,
	        [5] = scale_y,
	        [10] = ((renderer->far_plane + renderer->near_plane) / frustrum_length),
	        [11] = -1,
	        [14] = ((2 * renderer->near_plane * renderer->far_plane) / frustrum_length)},
	};

	return m;
}

struct layman_renderer *layman_renderer_create(void) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	renderer->pbr_shader = layman_shader_load_from_file("shaders/pbr.vert", "shaders/pbr.frag");
	if (!renderer->pbr_shader) {
		free(renderer);
		return NULL;
	}

	// TODO: Dynamic dimensions.
	renderer->viewport_width = 1280;
	renderer->viewport_height = 720;

	// TODO: Change the FOV.
	renderer->fov = 0.610865f; // 70 FOV
	renderer->far_plane = 1000.0f;
	renderer->near_plane = 0.1f;

	renderer->start_time = glfwGetTime();
	renderer->viewProjectionMatrixLocation = -1;
	renderer->sceneRotationMatrixLocation = -1;

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	free(renderer);
}

void layman_renderer_use(struct layman_renderer *renderer) {
	layman_shader_use(renderer->pbr_shader);
	glClearColor(0, 0, 0, 1); // Black.

	// FIXME: Camel case, and not a good way to obtain uniform locations. Should be here.
	GLint programId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
	renderer->viewProjectionMatrixLocation = glGetUniformLocation(programId, "viewProjectionMatrix");
	renderer->sceneRotationMatrixLocation = glGetUniformLocation(programId, "sceneRotationMatrix");

	// FIXME: Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glFrontFace(GL_CW);
	//glFrontFace(GL_CCW);
}

void layman_renderer_unuse(struct layman_renderer *renderer) {
	layman_shader_unuse(renderer->pbr_shader);
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_scene *scene) {
	(void) renderer; // Unused.

	double current_time = glfwGetTime();
	double elapsed = current_time - renderer->start_time;

	struct layman_matrix_4f projectionMatrix = calculate_projection_matrix(renderer);
	glUniformMatrix4fv(renderer->viewProjectionMatrixLocation, 1, false, projectionMatrix.d); // TODO: Missing view matrix?
	struct layman_matrix_4f sceneRotationMatrix = layman_matrix_4f_identity();
	layman_matrix_4f_rotate_x(&sceneRotationMatrix, M_PI);
	layman_matrix_4f_rotate_y(&sceneRotationMatrix, M_PI * elapsed * 0.5f);
	layman_matrix_4f_translate(&sceneRotationMatrix, LAYMAN_VECTOR_3F(0, 0, -3));
	glUniformMatrix4fv(renderer->sceneRotationMatrixLocation, 1, false, sceneRotationMatrix.d);

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render entities.
	layman_scene_render_entities(scene);
}
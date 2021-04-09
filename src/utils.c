#include "client.h"

float utils_closest_distance_between_ray_and_circle(vec3 ray_origin, vec3 ray_direction, vec3 circle_origin, vec3 circle_orientation, float circle_radius, vec3 closest) {
	vec3 plane_origin, plane_orientation;
	glm_vec3_copy(circle_origin, plane_origin);
	glm_vec3_copy(circle_orientation, plane_orientation);

	float t;

	if (utils_ray_plane_intersection(plane_origin, plane_orientation, ray_origin, ray_direction, &t)) {
		// Find the ray intersection point on the plane that contains the circle.
		vec3 on_plane;
		glm_vec3_copy(ray_origin, on_plane);
		glm_vec3_muladds(ray_direction, t, on_plane);

		// Project that intersection on to the circle's circumference.
		glm_vec3_sub(on_plane, circle_origin, closest);
		glm_normalize(closest);
		glm_vec3_mul(closest, (vec3) { circle_radius, circle_radius, circle_radius}, closest);
		glm_vec3_add(closest, circle_origin, closest);

		vec3 tmp;
		glm_vec3_sub(on_plane, closest, tmp);
		return glm_vec3_norm(tmp);
	}

	return -1;
}

bool utils_ray_plane_intersection(vec3 plane_origin, vec3 plane_normal, vec3 ray_origin, vec3 ray_direction, float *t) {
	float denom = glm_dot(plane_normal, ray_direction);

	if (fabs(denom) > 0.000001) {
		vec3 between;
		glm_vec3_sub(plane_origin, ray_origin, between);
		float result = glm_dot(between, plane_normal) / denom;
		*t = result;
		return result >= 0;
	}

	return false;
}

float utils_closest_distance_between_two_rays(vec3 r1_origin, vec3 r1_direction, vec3 r2_origin, vec3 r2_direction, float *d1, float *d2) {
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

void utils_render_cube(void) {
	static GLuint cubeVAO, cubeVBO;

	// initialize (if necessary)
	if (cubeVAO == 0) {
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *) (3 * sizeof (float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *) (6 * sizeof (float)));
	}

	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void utils_render_ngon(int n, float radius, mat4 transform, vec4 color) {
	float theta = (M_PI * 2) / (float) n;

	for (int i = 0; i < n; i++) {
		vec3 p1 = {0, radius, 0};
		vec3 p2 = {0, radius, 0};

		glm_vec3_rotate(p1, theta * i, (vec3) { 0, 0, 1});
		glm_vec3_rotate(p2, theta * (i + 1), (vec3) { 0, 0, 1});

		utils_render_line(p1, p2, transform, color);
	}
}

void utils_render_line(vec3 from, vec3 to, mat4 transform, vec4 color) {
	static GLuint vao, vbo;

	static float vertices[6] = {0};

	vertices[0] = from[0];
	vertices[1] = from[1];
	vertices[2] = from[2];
	vertices[3] = to[0];
	vertices[4] = to[1];
	vertices[5] = to[2];

	if (vao == 0) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}

	glUseProgram(client.renderer.plain_shader->program_id);

	static GLint color_location = -1;

	if (color_location == -1) {
		color_location = glGetUniformLocation(client.renderer.plain_shader->program_id, "u_Color");
	}

	glEnable(GL_LINE_SMOOTH);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);

	mat4 view_projection_matrix;
	glm_mat4_mul(client.renderer.projection_matrix, client.camera.view_matrix, view_projection_matrix);

	shader_bind_uniform_mvp(client.renderer.plain_shader, view_projection_matrix, transform, 1);
	glUniform4f(color_location, color[0], color[1], color[2], color[3]);

	glDrawArrays(GL_LINES, 0, 2);
}

struct entity *find_selected_entity(void) {
	if (client.ui.selected_entity_id == 0) {
		return NULL;
	}

	for (size_t i = 0; i < client.scene.entity_count; i++) {
		if (client.scene.entities[i]->id == client.ui.selected_entity_id) {
			return client.scene.entities[i];
		}
	}

	return NULL;
}

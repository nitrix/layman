#include "layman.h"

// TODO: Mesh cube.
GLuint cubeVAO, cubeVBO;
void renderCube() {
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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

static struct layman_texture *convert_equirectangular_to_cubemap(const struct layman_texture *equirectangular) {
	// Load & convert equirectangular environment map to a cubemap texture.
	struct layman_shader *equirect2cube_shader = layman_shader_load_from_files("shaders/equirect2cube/main.vert", "shaders/equirect2cube/main.frag", NULL);
	if (!equirect2cube_shader) {
		return NULL;
	}

	layman_shader_switch(equirect2cube_shader);

	int width = 1024, height = 1024;

	struct layman_framebuffer *fb = layman_framebuffer_create(width, height);
	if (!fb) {
		layman_shader_destroy(equirect2cube_shader);
		return NULL;
	}

	GLuint cubemap_id;
	glGenTextures(1, &cubemap_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
	for (size_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	mat4 projection;
	glm_perspective(90, 1, 0.1, 10.0, projection);

	mat4 views[6];
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 1, 0, 0}, (vec3) { 0, -1, 0}, views[0]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { -1, 0, 0}, (vec3) { 0, -1, 0}, views[1]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 1, 0}, (vec3) { 0, 0, 1}, views[2]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, -1, 0}, (vec3) { 0, 0, -1}, views[3]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, 1}, (vec3) { 0, -1, 0}, views[4]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, -1}, (vec3) { 0, -1, 0}, views[5]);

	// convert HDR equirectangular environment map to cubemap equivalent
	layman_shader_switch(equirect2cube_shader);
	GLint equirectangular_map_location = glGetUniformLocation(equirect2cube_shader->program_id, "equirectangularMap");
	glUniform1i(equirectangular_map_location, 0);
	GLint projection_location = glGetUniformLocation(equirect2cube_shader->program_id, "projection");
	glUniformMatrix4fv(projection_location, 1, false, (float *) projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangular->id);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
	for (unsigned int i = 0; i < 6; ++i) {
		GLint view_location = glGetUniformLocation(equirect2cube_shader->program_id, "view");
		glUniformMatrix4fv(view_location, 1, false, (float *) views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube();
	}

	// --------------------------------------------------------------------------------------

	layman_shader_destroy(equirect2cube_shader);

	struct layman_texture *cubemap = malloc(sizeof *cubemap);
	cubemap->id = cubemap_id;

	return cubemap;
}

struct layman_environment *layman_environment_create_from_hdr(const char *filepath) {
	struct layman_environment *environment = malloc(sizeof *environment);
	if (!environment) {
		return NULL;
	}

	environment->lambertian = NULL;
	environment->lambertian_lut = NULL;
	environment->ggx = NULL;
	environment->ggx_lut = NULL;
	environment->charlie = NULL;
	environment->charlie_lut = NULL;

	struct layman_texture *equirectangular = layman_texture_create_from_file(LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR, filepath);
	if (!equirectangular) {
		free(environment);
		return NULL;
	}

	struct layman_texture *cubemap = convert_equirectangular_to_cubemap(equirectangular);
	if (!cubemap) {
		layman_texture_destroy(equirectangular);
		free(environment);
	}

	layman_texture_destroy(cubemap);

	layman_texture_destroy(equirectangular);

	// TODO: Compute pre-filtered specular environment map.
	// TODO: Compute diffuse irradiance cubemap.
	// TODO: Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.'

	return environment;
}

void layman_environment_destroy(struct layman_environment *environment) {
	layman_texture_destroy(environment->lambertian);
	layman_texture_destroy(environment->lambertian_lut);
	layman_texture_destroy(environment->ggx);
	layman_texture_destroy(environment->ggx_lut);
	layman_texture_destroy(environment->charlie);
	layman_texture_destroy(environment->charlie_lut);

	free(environment);
}

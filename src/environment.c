#include "layman.h"

// TODO: Mesh cube.
void renderCube() {
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

static struct layman_texture *convert_equirectangular_to_cubemap(const struct layman_texture *equirectangular) {
	// Load & convert equirectangular environment map to a cubemap texture.
	struct layman_shader *equirect2cube_shader = layman_shader_load_from_files("shaders/equirect2cube/main.vert", "shaders/equirect2cube/main.frag", NULL);
	if (!equirect2cube_shader) {
		return NULL;
	}

	layman_shader_switch(equirect2cube_shader);

	int width = 2048, height = 2048;

	struct layman_framebuffer *fb = layman_framebuffer_create(width, height);
	if (!fb) {
		layman_shader_destroy(equirect2cube_shader);
		return NULL;
	}

	GLuint cubemap_id;
	glGenTextures(1, &cubemap_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
	for (size_t face = 0; face < 6; face++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	mat4 projection;
	glm_perspective(glm_rad(90), 1, 0.1, 1000.0, projection);

	mat4 views[6];
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 1, 0, 0}, (vec3) { 0, -1, 0}, views[0]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { -1, 0, 0}, (vec3) { 0, -1, 0}, views[1]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 1, 0}, (vec3) { 0, 0, 1}, views[2]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, -1, 0}, (vec3) { 0, 0, -1}, views[3]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, 1}, (vec3) { 0, -1, 0}, views[4]);
	glm_lookat((vec3) { 0, 0, 0}, (vec3) { 0, 0, -1}, (vec3) { 0, -1, 0}, views[5]);

	layman_shader_switch(equirect2cube_shader);
	GLint equirectangular_map_location = glGetUniformLocation(equirect2cube_shader->program_id, "equirectangularMap");
	glUniform1i(equirectangular_map_location, 0);
	GLint projection_location = glGetUniformLocation(equirect2cube_shader->program_id, "projection");
	glUniformMatrix4fv(projection_location, 1, false, (float *) projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangular->gl_id);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
	for (unsigned int i = 0; i < 6; ++i) {
		GLint view_location = glGetUniformLocation(equirect2cube_shader->program_id, "view");
		glUniformMatrix4fv(view_location, 1, false, (float *) views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube();
	}

	layman_shader_destroy(equirect2cube_shader);

	// TODO: This should use the texture module and not be created manually here.
	struct layman_texture *cubemap = malloc(sizeof *cubemap);
	cubemap->gl_id = cubemap_id;
	cubemap->gl_unit = GL_TEXTURE0 + LAYMAN_TEXTURE_KIND_CUBEMAP;
	cubemap->kind = LAYMAN_TEXTURE_KIND_CUBEMAP;
	cubemap->gl_target = GL_TEXTURE_CUBE_MAP;

	return cubemap;
}

struct layman_environment *layman_environment_create_from_hdr(const char *filepath) {
	struct layman_environment *environment = malloc(sizeof *environment);
	if (!environment) {
		return NULL;
	}

	struct layman_texture *equirectangular = layman_texture_create_from_file(LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR, filepath);
	if (!equirectangular) {
		free(environment);
		return NULL;
	}

	environment->cubemap = convert_equirectangular_to_cubemap(equirectangular);
	if (!environment->cubemap) {
		layman_texture_destroy(equirectangular);
		free(environment);
	}

	layman_texture_destroy(equirectangular);

	struct layman_shader *iblsampler_shader = layman_shader_load_from_files("shaders/iblsampler/main.vert", "shaders/iblsampler/main.frag", NULL);
	if (!iblsampler_shader) {
		fprintf(stderr, "Unable to load iblsampler shader\n");
		layman_environment_destroy(environment);
		return NULL;
	}

	layman_shader_switch(iblsampler_shader);

	GLint pfp_roughness_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_roughness");
	GLint pfp_samplecount_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_sampleCount");
	GLint pfp_miplevel_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_currentMipLevel");
	GLint pfp_width_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_width");
	GLint pfp_lodbias_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_lodBias");
	GLint pfp_distribution_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_distribution");

	struct layman_framebuffer *fb = layman_framebuffer_create(2048, 2048);
	if (!fb) {
		fprintf(stderr, "FB error\n");
		return NULL;
	}

	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

	GLuint cubemap_id;
	glGenTextures(1, &cubemap_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
	for (size_t face = 0; face < 6; face++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, NULL);
	}

	for (size_t face = 0; face < 6; face++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + face, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemap_id, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	layman_texture_switch(environment->cubemap);
	GLint cubemap_location = glGetUniformLocation(iblsampler_shader->program_id, "uCubeMap");
	glUniform1i(cubemap_location, environment->cubemap->kind);

	glUniform1f(pfp_roughness_location, 0.20);
	glUniform1ui(pfp_samplecount_location, 2);
	glUniform1ui(pfp_miplevel_location, 0);
	glUniform1ui(pfp_width_location, 2048); // FIXME
	glUniform1f(pfp_lodbias_location, 0);
	glUniform1ui(pfp_distribution_location, 1);

	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
	};

	glDrawBuffers(6, buffers);

	glBindFragDataLocation(iblsampler_shader->program_id, 0, "outFace0");
	glBindFragDataLocation(iblsampler_shader->program_id, 1, "outFace1");
	glBindFragDataLocation(iblsampler_shader->program_id, 2, "outFace2");
	glBindFragDataLocation(iblsampler_shader->program_id, 3, "outFace3");
	glBindFragDataLocation(iblsampler_shader->program_id, 4, "outFace4");
	glBindFragDataLocation(iblsampler_shader->program_id, 5, "outFace5");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEnable(GL_CULL_FACE);

	// TODO: Compute pre-filtered specular environment map.
	// TODO: Compute diffuse irradiance cubemap.
	// TODO: Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
	environment->lambertian = NULL;
	environment->lambertian_lut = NULL;
	environment->ggx = NULL;
	environment->ggx_lut = NULL;
	environment->charlie = NULL;
	environment->charlie_lut = NULL;

	layman_shader_destroy(iblsampler_shader);

	return environment;
}

void layman_environment_destroy(struct layman_environment *environment) {
	layman_texture_destroy(environment->cubemap);

	layman_texture_destroy(environment->lambertian);
	layman_texture_destroy(environment->lambertian_lut);
	layman_texture_destroy(environment->ggx);
	layman_texture_destroy(environment->ggx_lut);
	layman_texture_destroy(environment->charlie);
	layman_texture_destroy(environment->charlie_lut);

	free(environment);
}

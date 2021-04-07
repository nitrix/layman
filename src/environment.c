#include "client.h"

// FIXME: This needs to embedded the newer files on changes, maybe even hot-reload?
INCBIN(shaders_equirect2cube_main_vert, "../shaders/equirect2cube/main.vert");
INCBIN(shaders_equirect2cube_main_frag, "../shaders/equirect2cube/main.frag");
INCBIN(shaders_iblsampler_main_vert, "../shaders/iblsampler/main.vert");
INCBIN(shaders_iblsampler_main_frag, "../shaders/iblsampler/main.frag");

static bool convert_equirectangular_to_cubemap(const struct texture *equirectangular, struct texture *cubemap) {
	// Load & convert equirectangular environment map to a cubemap texture.
	struct shader *equirect2cube_shader = shader_load_from_memory(NULL,
			shaders_equirect2cube_main_vert_data, shaders_equirect2cube_main_vert_size,
			shaders_equirect2cube_main_frag_data, shaders_equirect2cube_main_frag_size,
			NULL, 0
	        );

	if (!equirect2cube_shader) {
		return false;
	}

	glUseProgram(equirect2cube_shader->program_id);

	int width = 2048, height = 2048;

	struct framebuffer fb;

	framebuffer_init(&fb, width, height);

	// FIXME: The code below is doing a lot of OpenGL things directly instead of working with our abstractions.

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

	glUseProgram(equirect2cube_shader->program_id);
	GLint equirectangular_map_location = glGetUniformLocation(equirect2cube_shader->program_id, "equirectangularMap");
	glUniform1i(equirectangular_map_location, 0);
	GLint projection_location = glGetUniformLocation(equirect2cube_shader->program_id, "projection");
	glUniformMatrix4fv(projection_location, 1, false, (float *) projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangular->gl_id);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
	for (unsigned int i = 0; i < 6; ++i) {
		GLint view_location = glGetUniformLocation(equirect2cube_shader->program_id, "view");
		glUniformMatrix4fv(view_location, 1, false, (float *) views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		utils_render_cube();
	}

	shader_destroy(equirect2cube_shader);

	// TODO: This should use the texture module and not be created manually here.
	cubemap->gl_id = cubemap_id;
	cubemap->gl_unit = TEXTURE_KIND_CUBEMAP;
	cubemap->kind = TEXTURE_KIND_CUBEMAP;
	cubemap->gl_target = GL_TEXTURE_CUBE_MAP;
	cubemap->width = width;

	// Cleanup.
	framebuffer_fini(&fb);

	return true;
}

bool environment_init_from_file(struct environment *environment, const char *filepath) {
	struct texture equirectangular;
	if (!texture_init_from_file(&equirectangular, TEXTURE_KIND_EQUIRECTANGULAR, filepath)) {
		return false;
	}

	if (!convert_equirectangular_to_cubemap(&equirectangular, &environment->cubemap)) {
		texture_fini(&equirectangular);
		return false;
	}

	texture_fini(&equirectangular);

	struct shader *iblsampler_shader = shader_load_from_memory(NULL,
			shaders_iblsampler_main_vert_data, shaders_iblsampler_main_vert_size,
			shaders_iblsampler_main_frag_data, shaders_iblsampler_main_frag_size,
			NULL, 0
	        );

	if (!iblsampler_shader) {
		return false;
	}

	environment->mip_count = 10;
	int sample_count = 1024;
	size_t width = 1024, height = 1024;

	glUseProgram(iblsampler_shader->program_id);

	GLint pfp_roughness_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_roughness");
	GLint pfp_samplecount_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_sampleCount");
	GLint pfp_miplevel_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_currentMipLevel");
	GLint pfp_width_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_width");
	GLint pfp_lodbias_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_lodBias");
	GLint pfp_distribution_location = glGetUniformLocation(iblsampler_shader->program_id, "pfp_distribution");

	struct framebuffer fb;
	framebuffer_init(&fb, width, height);

	// FIXME: The code below does a lot of OpenGL things without using our nice abstractions.

	glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
	glViewport(0, 0, width, height);

	// Lambertian
	GLuint lambertian_id;
	glGenTextures(1, &lambertian_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, lambertian_id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, environment->mip_count - 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	for (size_t mip = 0; mip < environment->mip_count; mip++) {
		for (size_t face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, GL_RGBA16F, width >> mip, height >> mip, 0, GL_RGBA, GL_FLOAT, NULL);
		}
	}

	// GGX
	GLuint ggx_id;
	glGenTextures(1, &ggx_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ggx_id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, environment->mip_count - 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	for (size_t mip = 0; mip < environment->mip_count; mip++) {
		for (size_t face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, GL_RGBA16F, width >> mip, height >> mip, 0, GL_RGBA, GL_FLOAT, NULL);
		}
	}

	GLuint ggx_lut_id;
	glGenTextures(1, &ggx_lut_id);
	glBindTexture(GL_TEXTURE_2D, ggx_lut_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Charlie
	GLuint charlie_id;
	glGenTextures(1, &charlie_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, charlie_id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, environment->mip_count - 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	for (size_t mip = 0; mip < environment->mip_count; mip++) {
		for (size_t face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, GL_RGBA16F, width >> mip, height >> mip, 0, GL_RGBA, GL_FLOAT, NULL);
		}
	}

	GLuint charlie_lut_id;
	glGenTextures(1, &charlie_lut_id);
	glBindTexture(GL_TEXTURE_2D, charlie_lut_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	texture_switch(&environment->cubemap);
	GLint cubemap_location = glGetUniformLocation(iblsampler_shader->program_id, "uCubeMap");
	glUniform1i(cubemap_location, environment->cubemap.gl_unit);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1ui(pfp_samplecount_location, sample_count);
	glUniform1ui(pfp_width_location, width);
	// glUniform1ui(pfp_width_location, environment->cubemap->width); // FIXME: The cubemap width or current mip?
	glUniform1f(pfp_lodbias_location, 0);

	const GLenum buffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6, // LUT
	};

	glDrawBuffers(7, buffers);

	glBindFragDataLocation(iblsampler_shader->program_id, 0, "outFace0");
	glBindFragDataLocation(iblsampler_shader->program_id, 1, "outFace1");
	glBindFragDataLocation(iblsampler_shader->program_id, 2, "outFace2");
	glBindFragDataLocation(iblsampler_shader->program_id, 3, "outFace3");
	glBindFragDataLocation(iblsampler_shader->program_id, 4, "outFace4");
	glBindFragDataLocation(iblsampler_shader->program_id, 5, "outFace5");
	glBindFragDataLocation(iblsampler_shader->program_id, 6, "outLUT");

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// glDisable(GL_CULL_FACE);

	for (int mip = environment->mip_count - 1; mip != -1; mip--) {
		glUniform1f(pfp_roughness_location, (float) mip / (float) (environment->mip_count - 1));
		glUniform1ui(pfp_miplevel_location, mip);

		// Lambertian
		glUniform1ui(pfp_distribution_location, 0);
		for (size_t face = 0; face < 6; face++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + face, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lambertian_id, mip);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, 0, 0); // Detach LUT.

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// GGX
		glUniform1ui(pfp_distribution_location, 1);
		for (size_t face = 0; face < 6; face++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + face, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, ggx_id, mip);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, ggx_lut_id, 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Charlie
		glUniform1ui(pfp_distribution_location, 2);
		for (size_t face = 0; face < 6; face++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + face, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, charlie_id, mip);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, charlie_lut_id, 0);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	// glEnable(GL_CULL_FACE);

	// FIXME: These are bypassing texture_init and I don't like it.

	environment->lambertian.gl_id = lambertian_id;
	environment->lambertian.kind = TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN;
	environment->lambertian.gl_unit = TEXTURE_KIND_ENVIRONMENT_LAMBERTIAN;
	environment->lambertian.gl_target = GL_TEXTURE_CUBE_MAP;

	environment->ggx.gl_id = ggx_id;
	environment->ggx.kind = TEXTURE_KIND_ENVIRONMENT_GGX;
	environment->ggx.gl_unit = TEXTURE_KIND_ENVIRONMENT_GGX;
	environment->ggx.gl_target = GL_TEXTURE_CUBE_MAP;

	environment->ggx_lut.gl_id = ggx_lut_id;
	environment->ggx_lut.kind = TEXTURE_KIND_ENVIRONMENT_GGX_LUT;
	environment->ggx_lut.gl_unit = TEXTURE_KIND_ENVIRONMENT_GGX_LUT;
	environment->ggx_lut.gl_target = GL_TEXTURE_2D;

	environment->charlie.gl_id = charlie_id;
	environment->charlie.kind = TEXTURE_KIND_ENVIRONMENT_CHARLIE;
	environment->charlie.gl_unit = TEXTURE_KIND_ENVIRONMENT_CHARLIE;
	environment->charlie.gl_target = GL_TEXTURE_CUBE_MAP;

	environment->charlie_lut.gl_id = charlie_lut_id;
	environment->charlie_lut.kind = TEXTURE_KIND_ENVIRONMENT_CHARLIE_LUT;
	environment->charlie_lut.gl_unit = TEXTURE_KIND_ENVIRONMENT_CHARLIE_LUT;
	environment->charlie_lut.gl_target = GL_TEXTURE_2D;

	// Cleanup.
	framebuffer_fini(&fb);
	shader_destroy(iblsampler_shader);

	return environment;
}

void environment_fini(struct environment *environment) {
	texture_fini(&environment->cubemap);
	texture_fini(&environment->lambertian);
	texture_fini(&environment->ggx);
	texture_fini(&environment->ggx_lut);
	texture_fini(&environment->charlie);
	texture_fini(&environment->charlie_lut);
}

void environment_switch(const struct environment *new) {
	texture_switch(&new->lambertian);
	texture_switch(&new->ggx);
	texture_switch(&new->ggx_lut);
	texture_switch(&new->charlie);
	texture_switch(&new->charlie_lut);
}

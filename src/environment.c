#include "layman.h"
#include "stb_image.h"
#include <stdlib.h>

/*
   static struct layman_texture *convert_equirectangular_to_cubemap(const struct layman_texture *equirectangular) {
        // Load & convert equirectangular environment map to a cubemap texture.
        struct layman_shader *equirect2cube_shader = layman_shader_load_from_files(NULL, NULL, "shaders/equirect2cube/main.comp");
        if (!equirect2cube_shader) {
                free(cubemap);
                return NULL;
        }

        layman_shader_switch(equirect2cube_shader);

        glBindTextureUnit(0, equirectangular->id);
        glBindImageTexture(0, equirectangular->id, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(equirectangular->width / 32, equirectangular->height / 32, 6);

        layman_shader_destroy(equirect2cube_shader);

        glGenerateTextureMipmap(cubemap->id);

        return cubemap;
   }
 */

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

	environment->equirectangular = layman_texture_create_from_file(LAYMAN_TEXTURE_KIND_EQUIRECTANGULAR, filepath);
	if (!environment->equirectangular) {
		free(environment);
		return NULL;
	}

	/*
	   TODO: The cubemap stuff.
	   struct layman_texture *cubemap = convert_equirectangular_to_cubemap(equirectangular);
	   if (!cubemap) {
	        layman_texture_destroy(equirectangular);
	        free(environment);
	   }
	 */

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

	// layman_texture_destroy(environment->equirectangular);

	free(environment);
}

void layman_environment_debug(const struct layman_environment *environment) {
	// enum layman_texture_kind kind = LAYMAN_TEXTURE_KIND_ALBEDO;
	// glActiveTexture(GL_TEXTURE0 + kind);
	// glBindTexture(GL_TEXTURE_2D, environment->equirectangular->id);
}

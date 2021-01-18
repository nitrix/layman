#ifndef LAYMAN_MODEL_H
#define LAYMAN_MODEL_H

struct layman_model {
	struct layman_mesh **meshes;
	size_t meshes_count;
};

struct layman_model *layman_model_load(const char *filepath);
void layman_model_destroy(struct layman_model *model);

#endif
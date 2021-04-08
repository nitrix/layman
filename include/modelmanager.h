#ifndef MODELMANAGER_H
#define MODELMANAGER_H

struct model *modelmanager_load_model(const char *filepath);
void modelmanager_unload_model(const struct model *model);

#endif

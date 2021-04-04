#ifndef MODELMANAGER_H
#define MODELMANAGER_H

struct model *modelmanager_use_model(const char *filepath);
void modelmanager_unuse_model(const struct model *model);

#endif

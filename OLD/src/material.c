#include "material.h"
#include "toolkit.h"

struct material *material_create(void) {
    struct material *material = malloc(sizeof *material);

    if (!material) {
        return NULL;
    }

    return material;
}

void material_destroy(struct material *material) {
    free(material);
}

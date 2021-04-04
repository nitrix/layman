#ifndef ENTITY_H
#define ENTITY_H

struct entity {
	uint32_t id;
	const struct model *model;
	vec3 translation;
	versor rotation;
	float scale;
};

bool entity_init(struct entity *entity, const char *model_filepath);
void entity_fini(struct entity *entity);
void entity_id_as_color(uint32_t id, vec4 color);
uint32_t entity_color_as_id(vec4 color);

#endif

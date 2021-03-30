#include "state.h"
#include <stdlib.h>

struct state state = {
	.window = NULL,
	.renderer = NULL,
	.camera = NULL,
	.scene = NULL,
	.environment = NULL,
};

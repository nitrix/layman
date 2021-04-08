#ifndef TOOLKIT_BUFFER_H
#define TOOLKIT_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

struct tk_buffer {
	void *data;
	size_t used;
	size_t capacity;
};

void tk_buffer_init(struct tk_buffer *buffer);
void tk_buffer_fini(struct tk_buffer *buffer);

bool tk_buffer_append(struct tk_buffer *buffer, const void *data, size_t size);
bool tk_buffer_append_format(struct tk_buffer *buffer, const char *format, ...);
bool tk_buffer_null_terminate(struct tk_buffer *buffer);
void tk_buffer_trim_null_terminator(struct tk_buffer *buffer);

#endif

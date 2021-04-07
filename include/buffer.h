#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

struct buffer {
	void *data;
	size_t used;
	size_t capacity;
};

void buffer_init(struct buffer *buffer);
void buffer_fini(struct buffer *buffer);

bool buffer_append(struct buffer *buffer, const void *data, size_t size);
bool buffer_append_format(struct buffer *buffer, const char *format, ...);
bool buffer_null_terminate(struct buffer *buffer);
void buffer_trim_null_terminator(struct buffer *buffer);

#endif

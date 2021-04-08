#include "toolkit.h"

void tk_buffer_init(struct tk_buffer *buffer) {
	buffer->data = NULL;
	buffer->used = 0;
	buffer->capacity = 0;
}

void tk_buffer_fini(struct tk_buffer *buffer) {
	free(buffer->data);
}

// Grows the buffer if not enough space for size.
static bool tk_grow_for_size_if_necessary(struct tk_buffer *buffer, size_t size) {
	if (size > buffer->capacity - buffer->used) {
		size_t new_capacity = buffer->capacity + size;
		void *new_data = realloc(buffer->data, new_capacity);
		if (!new_data) {
			return false;
		}

		buffer->data = new_data;
		buffer->capacity = new_capacity;
	}

	return true;
}

bool tk_buffer_append(struct tk_buffer *buffer, const void *data, size_t size) {
	if (!tk_grow_for_size_if_necessary(buffer, size)) {
		return false;
	}

	memcpy(buffer->data + buffer->used, data, size);
	buffer->used += size;

	return true;
}

bool tk_buffer_append_format(struct tk_buffer *buffer, const char *format, ...) {
	va_list args;
	va_start(args, format);

	size_t size = vsnprintf(NULL, 0, format, args) + 1;
	if (!tk_grow_for_size_if_necessary(buffer, size)) {
		return false;
	}

	vsnprintf(buffer->data + buffer->used, buffer->capacity - buffer->used, format, args);
	buffer->used += size;

	// Trim the null terminator added by vsnprintf.
	tk_buffer_trim_null_terminator(buffer);

	va_end(args);

	return true;
}

void tk_buffer_trim_null_terminator(struct tk_buffer *buffer) {
	const char *str = buffer->data;
	const char last_char = str[buffer->used - 1];

	if (last_char == '\0') {
		buffer->used--;
	}
}

bool tk_buffer_null_terminate(struct tk_buffer *buffer) {
	const char *str = buffer->data;
	const char last_char = str[buffer->used - 1];

	if (last_char == '\0') {
		return true;
	}

	return tk_buffer_append(buffer, "\0", 1);
}

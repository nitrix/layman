#include "toolkit.h"

char *tk_file_get_content(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		return NULL;
	}

	char *content = NULL;
	size_t content_size = 0;
	char line[1024];

	while (!feof(file)) {
		char *ok = fgets(line, sizeof line, file);
		if (!ok) {
			break;
		}

		size_t read = strlen(line);

		char *new_content = realloc(content, content_size + read);
		if (!new_content) {
			free(content);
			fclose(file);
			return NULL;
		}

		// Append to the new content.
		memcpy(new_content + content_size, line, read);

		content = new_content;
		content_size += read;
	}

	// Resize to final size and null terminate.

	char *new_content = realloc(content, content_size + 1);
	if (!new_content) {
		fclose(file);
		free(content);
		return NULL;
	}

	content = new_content;
	content[content_size] = '\0';

	fclose(file);

	return content;
}

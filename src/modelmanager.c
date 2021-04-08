#include "client.h"

struct entry {
	char *filepath;
	struct model *model; // Has to stay a pointer for address stability (we hand off these pointers).
	size_t uses;
};

struct modelmanager {
	struct entry *entries;
	size_t used;
	size_t capacity;
} mm;

struct model *modelmanager_load_model(const char *filepath) {
	// Ensure the entry doesn't already exist.
	for (size_t i = 0; i < mm.used; i++) {
		struct entry *entry = &mm.entries[i];
		if (strcmp(entry->filepath, filepath) == 0) {
			entry->uses++;
			return entry->model;
		}
	}

	// Load the model.
	struct model *model = model_load(filepath);
	if (!model) {
		return false;
	}

	// Prepare storage for the new entry if there isn't enough space.
	if (mm.used == mm.capacity) {
		size_t new_capacity = mm.capacity + 1;

		void *new_entries = realloc(mm.entries, new_capacity * sizeof *mm.entries);
		if (!new_entries) {
			return NULL;
		}

		mm.capacity = new_capacity;
		mm.entries = new_entries;
	}

	// Store the model.
	struct entry *entry = &mm.entries[mm.used++];

	entry->uses = 1;
	entry->model = model;
	entry->filepath = strdup(filepath);

	return model;
}

void modelmanager_unload_model(const struct model *model) {
	for (size_t i = 0; i < mm.used; i++) {
		struct entry *entry = &mm.entries[i];
		if (entry->model == model) {
			entry->uses--;

			// Check if this was the last usage of this entry. If so, time to cleanup.
			if (entry->uses == 0) {
				// Replace the current entry with the last entry, unless we are the last entry.
				bool is_last_entry = i == mm.used - 1;
				if (!is_last_entry) {
					*entry = mm.entries[mm.used - 1];
				}

				mm.used--;
			}
		}
	}
}

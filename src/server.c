#include "toolkit.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int server_run(void) {
	printf("%s (%s)\n", DEFAULT_TITLE, VERSION);
	printf("Listening on 0.0.0.0:5000\n");

	while (true) {
		time_millisleep(1000);
	}

	return EXIT_SUCCESS;
}

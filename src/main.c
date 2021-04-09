#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
	bool run_as_server = argc > 1 && strcmp(argv[1], "--server") == 0;

	if (run_as_server) {
		return server_run();
	} else {
		return client_run();
	}
}

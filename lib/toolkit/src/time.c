#include "toolkit.h"

#ifdef _WIN32
#include <windows.h>
#elif __unix
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#else
#error "Unknown system"
#endif

void time_millisleep(int msecs) {
	#ifdef _WIN32
	Sleep(msecs);
	#elif __unix
	struct timespec ts;
	ts.tv_sec = msecs / 1000;
	ts.tv_nsec = msecs % 1000 * 1000;
	nanosleep(&ts, NULL);
	#endif
}

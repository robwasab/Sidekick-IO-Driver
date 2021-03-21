
#include "sidekickio.h"

#ifdef _MSC_VER

#include <windows.h>

// eww....
void SidekickIO::delay_us(uint32_t micros) {
	__int64 waitTime = (__int64)micros;
	__int64 time1 = 0;
	__int64 time2 = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&time1);

	do {
		QueryPerformanceCounter((LARGE_INTEGER*)&time2);
	} while ((time2 - time1) < waitTime);
}


#else

#include <unistd.h>

void SidekickIO::delay_us(uint32_t micros) {
	usleep(micros);
}

#endif

#include "utils.h"
#include <stdio.h>

i64 deltatime(timespec t0, timespec t1) {
	return (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);
}

timespec gettime() {
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t;
}

void hexdump(void const *buffer, u32 size) {
	u8 const *bytes = (u8*)buffer;
	for (u32 i = 0; i < size; i++)
		printf("%02X ", bytes[i]);
}

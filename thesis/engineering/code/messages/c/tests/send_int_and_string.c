#define _POSIX_C_SOURCE 199309L
#include "colosseum.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int f = open(argv[1], O_WRONLY);
	char const *msg = "Hello, World!";

	while (1) {
		timespec t0 = gettime();
		if (msendf(f, 42, "%I %c[%]", 999, msg, strlen(msg)) <= 0) continue;
		timespec t1 = gettime();
		printf("%ldns elapsed\n", deltatime(t0, t1));
	}

	close(f);
	return 0;
}

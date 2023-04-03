#include "colosseum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	int f = open(argv[1], O_RDONLY);

	u8 buf[4096];
	i8 tag = 0;

	i32 num;
	char str[128];
	u32 str_len;

	// message loop
	while (1) {
		timespec t0 = gettime();
		if (mrecv(f, &tag, buf, 4096) <= 0) continue;
		mscanf(buf, "%I %c[%<=128]", &num, str, &str_len);
		timespec t1 = gettime();
		printf("tag %d <- (%d, %u, '%.*s')\n", tag, num, str_len, str_len, str);
		assert(num == 999);
		assert(str_len == 13);
		printf("%ldns elapsed\n", deltatime(t0, t1));
	}

	close(f);
	return 0;
}

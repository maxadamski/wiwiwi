#include "stdio.h" 
#include "stdlib.h" 
#include "colosseum.h"

int main(int argc, char **argv) {
	int f = open(argv[1], O_WRONLY);
	msize_t const size = 0x4000;
	char const data[size];
	msend(f, data, size, 0);
	close(f);
	return 0;
}

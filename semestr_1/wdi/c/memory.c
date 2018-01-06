#include <stdio.h>
#include <stdlib.h>

int main(void) {
	printf("sizeof(char) = %d\n", sizeof(char));
	printf("sizeof(float) = %d\n", sizeof(float));
	printf("sizeof(double) = %d\n", sizeof(double));
	printf("sizeof(short int) = %d\n", sizeof(short int));
	printf("sizeof(int) = %d\n", sizeof(int));
	printf("sizeof(uint) = %d\n", sizeof(unsigned int));
	printf("sizeof(long int) = %d\n", sizeof(long));
	int a = 5;
	int b = 4;
	int *pa = &a;

	*pa = 2;
	pa += sizeof(a);
	*pa = 1;

	printf("pa = %x\n", pa);
	printf("pa = %x\n", pa + sizeof(a));
	printf("a = %d\n", a);
	printf("b = %d\n", b);

	return 0;
}

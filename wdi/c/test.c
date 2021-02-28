#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int a[3];
	int b;
} S;

void test(S s) {
	s.a[0] = 0;
	printf("in test %d\n", s.a[0]);
}

int main(void) {
	S s;
	s.b = 30;
	s.a[0] = 1;
	s.a[1] = 2;
	s.a[2] = 3;
	test(s);
	printf("in main %d\n", s.a[0]);
	return 0;
}

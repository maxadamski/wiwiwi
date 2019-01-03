#include <stdio.h>
#include <stdlib.h>

int sil(int n) {
	return n <= 1 ? 1 : sil(n - 1) * n;
}

int nwt(int n, int k) { 
	return k == 0 || k == n ? 1 : nwt(n-1, k-1) + nwt(n-1, k);
}

double P(int n, double x) {
	if (n == 0) return 0;
	if (n == 1) return x;
	double dn = (double) n;
	return (2*dn+1)/(dn+1)*x*P(n-1, x) - dn/(dn+1)*P(n-2, x);
}

double H(int n, double x) {
	if (n == 0) return 1;
	if (n == 1) return 2*x;
	return 2*x*H(n-1, x) - 2*n*H(n-2, x);
}

int fib(int n) { // O(n)
	if (n < 0) return -1;
	int *F = malloc((n + 1) * sizeof(int));
	F[0] = 0; F[1] = 1;
	for (int i = 2; i <= n; i += 1)
		F[i] = F[i - 1] + F[i - 2];
	int res = F[n];
	free(F);
	return res;
}

int main(void) {
//	for (int i = 0; i < 100; i++)
//		printf("%d ", fib(i));
//	return 0;
}

#include <cstdio>
#include <cstdint>
#include <omp.h>

typedef uint64_t u64;
typedef int64_t  i64;
typedef double   f64;

int main() {
	i64 steps = 1e9;
	f64 step = 1.0 / steps;

	f64 t0 = omp_get_wtime();
	f64 x, sum = 0;
	for (i64 i = 0; i < steps; i++) {
		x = (i + 0.5) * step;
		sum = sum + 4.0 / (1.0 + x*x);
	}
	f64 pi = sum * step;
	f64 t1 = omp_get_wtime();

	printf("pi = %.12f\n", pi);
	printf("elapsed %.4f seconds\n", t1-t0);
}


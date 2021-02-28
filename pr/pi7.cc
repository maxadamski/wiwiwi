#include <cstdio>
#include <cstdint>
#include <omp.h>

typedef uint64_t u64;
typedef int64_t  i64;
typedef double   f64;

int main() {
	i64 steps = 1e9;
	f64 step = 1.0 / steps;

	omp_set_num_threads(2);
	const i64 runs = 50;
	volatile f64 sum[runs];

	for (i64 k = 0; k < runs-1; k++) {
		sum[k] = 0;
		sum[k+1] = 0;

		f64 t0 = omp_get_wtime();
		#pragma omp parallel
		{
			i64 id = omp_get_thread_num();
			#pragma omp for
			for (i64 i = 0; i < steps; i++) {
				f64 x = (i + 0.5) * step;
				sum[k+id] += 4.0 / (1.0 + x*x);
			}
		}
		f64 t1 = omp_get_wtime();
		f64 pi = (sum[k]+sum[k+1])*step;
		printf("elapsed %.4f seconds (run %d)\n", t1-t0, k);
	}
}


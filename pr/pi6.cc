#include <cstdio>
#include <cstdint>
#include <omp.h>

typedef uint64_t u64;
typedef int64_t  i64;
typedef double   f64;

const i64 max_threads = 4;
i64 threads[3] = {4,2,1};

int main() {
	i64 steps = 1e9;
	f64 step = 1.0 / steps;

	for (i64 i = 0; i < 3; i++) {
		printf("thread count = %d\n", threads[i]);
		omp_set_num_threads(threads[i]);
		f64 t0 = omp_get_wtime();

		volatile f64 sum[max_threads] = {0};
		#pragma omp parallel
		{
			i64 id = omp_get_thread_num();
			#pragma omp for
			for (i64 i = 0; i < steps; i++) {
				f64 x = (i + 0.5) * step;
				sum[id] += 4.0 / (1.0 + x*x);
			}
		}
		f64 total = 0;
		for (i64 i = 0; i < max_threads; i++) total += sum[i];
		f64 pi = total * step;
		f64 t1 = omp_get_wtime();

		printf("pi = %.12f\n", pi);
		printf("elapsed %.4f seconds\n", t1-t0);
		printf("---\n");
	}
}


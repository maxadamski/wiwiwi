#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define min(A, B) ((A < B) ? A : B)
#define max(A, B) ((A > B) ? A : B)

#define DEBUG false

int loss(int N,
		int * const restrict P,
		int * const restrict R,
		int * const restrict D,
		int * const restrict W,
		int * const restrict J) {
	int t = 0;
	int L = 0;
	for (int i = 0; i < N; i++) {
		int j = J[i];
		if (t < R[j]) t = R[j];
		t += P[j];
		if (t > D[j]) L += W[j];
	}
	return L;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s FILE\n", argv[0]);
		return 1;
	}
	FILE *fd = fopen(argv[1], "r");
	int N;
	fscanf(fd, "%d\n", &N);
	int P[N], R[N], D[N], W[N], S[N];
	int J[N], T[N], Tt[N];
	int T_size = N;

	int max_D = 0;
	for (int i = 0; i < N; i++) {
		fscanf(fd, "%d %d %d %d\n", P+i, R+i, D+i, W+i);
		S[i] = D[i] - R[i];
		max_D = max(max_D, D[i]);
		T[i] = i;
		J[i] = -1;
		Tt[i] = 0;
	}
	fclose(fd);

	clock_t time0 = clock();

	int M[++max_D];
	for (int i = 0; i < max_D; i++) M[i] = -1;

	while (T_size > 0) {
		int assigned = 0;
		int t = 0;
		while (t < max_D && T_size > 0) {
			int Tn = 0;
			for (int i = 0; i < T_size; i++) {
				int j = T[i];
				int t1 = max(t, R[j]);
				if (D[j] - t1 < P[j]) continue;
				int t2 = t1 + P[j];
				if (t2 >= max_D) continue;
				for (int i = t1; i < t2; i++) if (M[i] != -1) goto skip;
				Tt[Tn++] = j;
				skip: continue;
			}

			if (Tn == 0) {
				// skipping to min_R is acually slower...
				//int min_R = max_D;
				//for (int i = 0; i < T_size; i++) min_R = min(min_R, R[T[i]]);
				//t = max(t + 1, min_R);
				t++;
				continue;
			}

			int best_j = Tt[0];
			double best_score = 0;
			for (int i = 0; i < Tn; i++) {
				int j = Tt[i];
				double score = (double) W[j] / (D[j] - t);
				if (score >= best_score) {
					best_score = score;
					best_j = j;
				}
			}

			int t1 = max(t, R[best_j]);
			int t2 = t1 + P[best_j];
			t = t2;
			for (int i = 0; i < T_size; i++) {
				if (T[i] == best_j) {
					for (int j = i; j < T_size - 1; j++) T[j] = T[j+1];
					T_size--;
					break;
				}
			}
			for (int i = t1; i < t2; i++) M[i] = best_j;
			assigned++;
		}
		if (!assigned) break;
	}

	double dt = (double) 1e3 * (clock() - time0) / CLOCKS_PER_SEC;

	{
		int n = 0;
		int last_j = -1;
		int i = 0;

		while (i < max_D) {
			int j = M[i];
			if (j == -1) {
				i++;
				continue;
			}
			if (j != last_j) {
				J[n++] = j;
				last_j = j;
				i += P[j];
			}
		}

		for (int i = 0; i < T_size; i++) {
			J[n++] = T[i];
		}

		if (n != N) {
			printf("not all jobs were scheduled! (%d != %d)\n", n, N);
		}
	}

	int L = loss(N, P, R, D, W, J);

	printf("%d\n", L);

	printf("%d", J[0] + 1);
	for (int i = 1; i < N; i++)
		printf(" %d", J[i] + 1);

	fprintf(stderr, "%d\n", L);
	//fprintf(stderr, "%d\n", (int) dt);
	//fprintf(stderr, "%d\t%d\t%d\t%s\n", N, L, dt, argv[1]);
}


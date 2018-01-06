#include <stdio.h>
#include <stdlib.h>

typedef struct Vertex Vertex;
typedef struct Path Path;

struct Path {
	int weight;
	Vertex *a, *b;
};

struct Vertex {
	// nazwa, kolor i inne duperele
	int name;
};

int main(void) {
	// statyczny model
	int graph[6][6] = {
		0,  4,  2,  0,  0,  0,
		4,  0,  5,  10, 0,  0,
		2,  5,  0,  0,  3,  0,
		0,  10, 0,  0,  4,  11,
		0,  0,  3,  4,  0,  0,
		0,  0,  0,  11, 0,  0
	};

	// dynamiczny model
	int N = 6;
	Vertex *vertices = calloc(N, sizeof(Vertex));
	int paths_n = 0;
	Path *paths;

	// uwaga: zakladamy, ze graf nie jest skierowany
	// konwersja statyczny -> dynamiczny
	for (int n = 0; n < N; n += 1) {
		vertices[n].name = n + 1;
		for (int m = 0; m < n; m += 1) {
			int weight = graph[m][n];
			if (weight != 0) {
				paths = realloc(paths, ++paths_n * sizeof(Path));
				paths[paths_n - 1] = (Path) { 
					weight, &vertices[m], &vertices[n]
				};
			}
		}
	}

	free(paths);
	free(vertices);

	printf("didn't crash = success!\n");

	return 0;
}

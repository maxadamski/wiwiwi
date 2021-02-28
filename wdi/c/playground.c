#include <stdio.h>
#include <stdlib.h>

typedef struct Path Path;
typedef struct Vertex Vertex;

struct Path {
	int weight;
	Vertex *next;
};

struct Vertex {
	int path_count;
	Path **paths;
};

int main(void) {
	Vertex graph_dynamic[6] = {{0}};
	int graph_static[6][6] = {
		0,  4,  2,  0,  0,  0,
		4,  0,  5,  10, 0,  0,
		2,  5,  0,  0,  3,  0,
		0,  10, 0,  0,  4,  11,
		0,  0,  3,  4,  0,  0,
		0,  0,  0,  11, 0,  0
	};

	for (int n = 0; n < 6; n += 1) {
		for (int m = 0; m < 6; m += 1) {
			int weight = graph_static[m][n];
			if (weight != 0) {
				Vertex *vertex = &graph_dynamic[m];
				Path **paths = vertex->paths;
				vertex->paths = realloc(vertex->paths, sizeof(vertex->paths) + sizeof(Path));
				vertex->path_count += 1;

				Path *path = malloc(sizeof(Path));
				path->weight = weight;
				path->next = &graph_dynamic[n];

				vertex->paths[vertex->path_count - 1] = path;
			}
		}
	}

	for (int i = 0; i < 6; i += 1) {
		Vertex *vertex = &graph_dynamic[i];
		for (int j = 0; j < vertex->path_count; j += 1) 
			free(vertex->paths[j]);
	}

	printf("didn't crash = success!\n");
	return 0;
}

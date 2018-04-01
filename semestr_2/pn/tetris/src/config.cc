#include "config.hh"
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cmath>

TetrominoShape tetromino_shape(FILE *file, int size) {
	assert(size == 3 || size == 4);
	TetrominoShape shape;

	// each rotation is a 4x4 binary matrix
	for (int y = 0; y < 4; y += 1) {
		for (int x = 0; x < 4; x += 1) {
			// if rotation is 3x3 fill right corner with zeros
			// but not if reading the I and O tetrominos
			if (size == 3 && (x == 3 || y == 0)) {
				shape[y][x] = 0;
			} else {
				int value;
				fscanf(file, "%d", &value);
				shape[y][x] = value;
			}
		}
	}

	for (auto row : shape) {
		for (auto col : row) {
			fprintf(stderr, "%d ", col);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");

	return shape;
}

RotationSystem rotation_system(std::string path) {
	RotationSystem rotation_system;
	FILE *file = fopen(path.c_str(), "r");
	TetrominoType keys[] = {
		'I', 'O', 'J', 'L', 'S', 'T', 'Z'
	};

	// read rotations for 7 tetrominos
	for (int i = 0; i < 7; i += 1) {
		TetrominoRotation rotation;
		// each tetromino has 4 rotations
		for (int j = 0; j < 4; j += 1) {
			int shape_size = i >= 2 ? 3 : 4;
			auto shape = tetromino_shape(file, shape_size);
			rotation[j] = shape;
		}
		rotation_system[keys[i]] = rotation;
	}

	fclose(file);
	return rotation_system;
}

ColorTheme color_theme(std::string path) {
	ColorTheme color_theme;
	FILE *file = fopen(path.c_str(), "r");

	// read colors for 7 tetrominoes
	for (int i = 0; i < 7; i += 1) {
		char line[64];
		fgets(line, 64, file);
		char key, color[8];
		sscanf(line, "%c %s", &key, color);
		// TODO: read the color
		color_theme[key] = sf::Color(0);
	}

	fclose(file);
	return color_theme;
}

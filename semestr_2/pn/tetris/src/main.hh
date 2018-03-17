#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>

#define FPS 60

#define DISPLAY_OPTIONS ALLEGRO_RESIZABLE | ALLEGRO_FULLSCREEN_WINDOW
#define FONT_OPTIONS ALLEGRO_TTF_MONOCHROME | ALLEGRO_TTF_NO_KERNING | ALLEGRO_TTF_NO_AUTOHINT

// fonts
#define FONT_KOURYUU_PATH "res/font/kouryuu.ttf"
#define FONT_KOURYUU_BASE 11

// Tetris matrix is 10 blocks wide,
// and at least 22 blocks high.
// Two topmost blocks are hidden.
#define MATRIX_W 10
#define MATRIX_H 22

// colors
#define WHITE   al_color_name("white")
#define BLACK   al_color_name("black")
#define CYAN    al_color_name("cyan")
#define YELLOW  al_color_name("yellow")
#define MAGENTA al_color_name("megenta")
#define GREEN   al_color_name("lightgreen")
#define RED     al_color_name("red")
#define BLUE    al_color_name("blue")
#define ORANGE  al_color_name("orange")

typedef char TetrominoType;
typedef std::array<std::array<bool, 4>, 4> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;
typedef std::map<TetrominoType, TetrominoRotation> RotationSystem;
typedef std::map<TetrominoType, ALLEGRO_COLOR> ColorTheme;

const TetrominoType tetromino_names[] = {
	'I', 'O', 'J', 'L', 'S', 'T', 'Z'
};

enum BlockType {
	BLOCK_NONE,
	BLOCK_GHOST,
	BLOCK_SOLID,
	BLOCK_FALLING,
};

TetrominoShape read_tetromino_shape(FILE *file, int size) {
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

RotationSystem read_rotation_system(std::string path) {
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
			auto shape = read_tetromino_shape(file, shape_size);
			rotation[j] = shape;
		}
		rotation_system[keys[i]] = rotation;
	}

	fclose(file);
	return rotation_system;
}

ColorTheme read_color_theme(std::string path) {
	ColorTheme color_theme;
	FILE *file = fopen(path.c_str(), "r");

	// read colors for 7 tetrominoes
	for (int i = 0; i < 7; i += 1) {
		char line[64];
		fgets(line, 64, file);
		char key, color[8];
		sscanf(line, "%c %s", &key, color);
		color_theme[key] = al_color_html(color);
	}

	fclose(file);
	return color_theme;
}

// Game objects



typedef struct Block Block;
typedef struct Tetromino Tetromino;
typedef struct Matrix Matrix;

struct Block {
	BlockType type;
	TetrominoType tetromino_type;
};

struct Tetromino {
	std::vector<Block*> children;
	TetrominoType type;
	int rotation;
};

void tetromino_rotate(Tetromino *tetromino) {
	tetromino->rotation += 1;
	tetromino->rotation %= 4;
}

struct Matrix {
	std::array<std::array<Block, MATRIX_W + 2>, MATRIX_H + 2> blocks;
	ColorTheme color_theme;
	RotationSystem rotation_system;
};

void matrix_init(Matrix *matrix);
void matrix_draw(Matrix *matrix);

// Geometry

typedef struct Point Point;
struct Point {
	double x, y;
};

typedef struct Size Size;
struct Size {
	double w, h;
};

// Helpers

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

#endif

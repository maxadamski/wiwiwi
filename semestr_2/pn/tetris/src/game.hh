#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "utils.hh"

#include <string>
#include <map>
#include <vector>
#include <optional>

typedef std::vector<std::vector<bool>> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;
typedef std::array<int, 3> RGBColor;

enum TetrominoType {
	I = 'I', O = 'O', J = 'J', L = 'L', S = 'S', T = 'T', Z = 'Z'
};

class RotationSystem {
	std::map<TetrominoType, TetrominoRotation> data;

	public:
		RotationSystem(std::string file_path);

		TetrominoShape get_shape(TetrominoType type, int rotation);
};

class ColorScheme {
	std::map<TetrominoType, RGBColor> data;

	public:
		ColorScheme(std::string file_path);

		RGBColor get_color(TetrominoType type);
};

class Block {
	TetrominoType type;
	Color color;
	// TODO: add sprite property

	public:
		Block();
};

class Matrix {
	std::vector<std::vector<std::optional<Block>>> data;
	Vec2 origin, size, block_size;
	int rotation;

	public:
		Matrix(Vec2 origin, Vec2 size);

		void rotate_right();
		void draw(Window &window);
};

class Board {
	Vec2 origin, size;
	Matrix board;

	public:
		Board(Vec2 origin, Vec2 size):
			origin(origin), size(size), board(Matrix(origin, size)) {};

		void draw(Window &window);
};

#endif

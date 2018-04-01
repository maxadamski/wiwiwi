#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "utils.hh"

#include <string>
#include <map>
#include <vector>
#include <optional>

typedef std::vector<std::vector<bool>> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;
enum TetrominoType {
	I = 'I', O = 'O', J = 'J', L = 'L', S = 'S', T = 'T', Z = 'Z',
	NONE
};

class RotationSystem {
	std::map<TetrominoType, TetrominoRotation> data;

	public:
		RotationSystem(std::string file_path);

		TetrominoShape get_shape(TetrominoType type, int rotation);
};

class ColorScheme {
	std::map<TetrominoType, Color> data;

	public:
		ColorScheme(std::string file_path);

		Color get_color(TetrominoType type);
};

class Block {
	public:
		Color color;
		// TODO: add sprite property

		Block(Color color): color(color) {};
};

typedef std::vector<std::vector<std::optional<Block>>> BlockMatrix;

class BlockFactory {
	RotationSystem rotation_system;
	ColorScheme color_scheme;

	public:
		BlockFactory(RotationSystem rotation_system, ColorScheme color_scheme):
			rotation_system(rotation_system), color_scheme(color_scheme) {};

		BlockMatrix get_blocks(TetrominoType type, int rotation);
};

class Matrix {
	BlockMatrix data;
	Vec2 origin, size, block_size;
	int rotation = 0;

	public:
		Matrix(Vec2 origin, Vec2 size, Vec2 block_size);

		void update_tetromino(TetrominoType type, BlockFactory &factory);
		void rotate_right();
		void draw(Window &window);
};

class Board {
	Vec2 origin, size, block_size;
	Matrix board;

	public:
		Board(Vec2 origin, Vec2 size, Vec2 block_size): 
			origin(origin), size(size), block_size(block_size),
			board(Matrix(origin, size, block_size)) {};

		void insert(Matrix matrix, Vec2 origin);
		void draw(Window &window);
};

#endif

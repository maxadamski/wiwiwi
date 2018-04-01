#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "utils.hh"

#include <string>
#include <map>
#include <vector>
#include <optional>

#define BLOCK_SIZE 32

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
	public:
		RotationSystem rotation_system;
		ColorScheme color_scheme;

		BlockFactory(RotationSystem rotation_system, ColorScheme color_scheme):
			rotation_system(rotation_system), color_scheme(color_scheme) {};

		BlockMatrix get_blocks(TetrominoType type, int rotation);
};

class Matrix {
	public:
		Vec2 origin, block_size;
		TetrominoType type;
		int rotation = 0;
		BlockMatrix data;

		Matrix(TetrominoType type, Vec2 origin, Vec2 size = Vec2(0, 0), 
			Vec2 block_size = Vec2(BLOCK_SIZE, BLOCK_SIZE));

		void update_tetromino(BlockFactory &factory);
		void rotate_right();
		void rotate_left();
		void draw(Window &window);
		bool collides(Matrix &matrix);
		Vec2 get_size();
};

class Board {
	public:
		Vec2 block_size;
		Matrix board;
		std::optional<Matrix> falling = std::nullopt;

		Board(Vec2 size, Vec2 block_size = Vec2(BLOCK_SIZE, BLOCK_SIZE)): 
			block_size(block_size),
			board(Matrix(NONE, Vec2(0, 0), size, block_size)) {};

		void insert(Matrix matrix, Vec2 origin);
		void draw(Window &window);
		bool contains(Matrix &matrix);
};

#endif

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "utils.hh"

#include <string>
#include <map>
#include <vector>
#include <optional>
#include <functional>

#define BOARD_COLOR 0xF0F5F9FF
#define FONT_COLOR 0xC9D6DFFF
#define BG_COLOR 0x1E2022FF
#define RED_COLOR 0xF15C5CFF
#define SPAWN_POS Vec2(3, -1)
#define BLOCK_SIZE 32
#define FREEZE 0.4
#define ACTION 0.05

typedef std::vector<std::vector<bool>> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;

enum TetrominoType {
	I = 'I', O = 'O', J = 'J', L = 'L', S = 'S', T = 'T', Z = 'Z',
	NONE
};

const std::vector<char> tetromino_types = {I, O, J, L, S, T, Z};

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

struct Block {
	Color color;
	bool shadow = false;
	// TODO: add sprite property

	Block(Color color): color(color) {};

	void draw(Window &window, Vec2 origin, Vec2 size);
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

class TetrominoFactory {
	public:
		virtual TetrominoType next() = 0;
};

class BaggedTetrominoFactory : public TetrominoFactory {
	std::optional<char> last = std::nullopt;
	std::vector<char> bag;

	public:
		TetrominoType next();
};

class RandomTetrominoFactory : public TetrominoFactory {
	public:
		TetrominoType next();
};

struct Collision {
	bool top = false,
		 right = false,
		 bottom = false,
		 left = false,
		 intersect = false;

	bool any();
};

struct State {
	Timer gravity = Timer(sf::seconds(0.8)),
		  freeze = Timer(sf::seconds(FREEZE)),
		  action = Timer(sf::seconds(ACTION));

	sf::Time elapsed_time = sf::seconds(0);
	int score = 0, lines = 0;
	bool game_over = false;

	int level();
	sf::Time turn_length();
	void update_gravity();
	void update(sf::Time elapsed);
	int elapsed_seconds();

	private:
		double turn_frames();
		std::vector<Timer*> timers();
};

class Matrix {
	Vec2 block_size = Vec2(BLOCK_SIZE, BLOCK_SIZE);
	BlockFactory factory;
	TetrominoType type;
	int rotation = 0;

	int get_width();
	int get_height();

	public:
		Vec2 origin = Vec2(0, 0);
		int hold_count = 0;
		BlockMatrix data;

		Matrix(TetrominoType type, BlockFactory &factory):
			factory(factory), type(type) {};

		void rotate_reset();
		void rotate_right();
		void rotate_left();
		void draw(Window &window, Vec2 offset = Vec2(0, 0));
		Collision collision(Matrix &board);
		Vec2 get_size();
		void set_size(Vec2 size);
		void update();
		bool valid_point(Vec2 point);
		bool was_on_hold();
};

class Board {
	Vec2 block_size = Vec2(BLOCK_SIZE, BLOCK_SIZE);
	TetrominoFactory &tetromino_factory;
	BlockFactory block_factory;

	void insert(Matrix matrix);

	public:
		Matrix falling;
		Matrix next;
		Matrix board;
		std::optional<Matrix> hold = std::nullopt;

		Board(Vec2 size, TetrominoFactory &tetromino_factory, BlockFactory &block_factory):
			tetromino_factory(tetromino_factory), block_factory(block_factory),
			falling(Matrix(tetromino_factory.next(), block_factory)),
			next(Matrix(tetromino_factory.next(), block_factory)),
			board(Matrix(NONE, block_factory)) {
			board.set_size(size);
			falling.update();
		}

		void draw(Window &window, 
				Vec2 offset = Vec2(0, 0), 
				bool shadow = true);
		void freeze();
		void spawn();
		void hold_swap();
		void hard_drop();
		bool can_move_right(Matrix m);
		bool can_move_left(Matrix m);
		bool can_move_down(Matrix m);
		bool can_rotate_right(Matrix m);
		bool can_rotate_left(Matrix m);
		bool game_over();
		std::vector<int> full_lines();
		void clear_row(int row);
};

#endif

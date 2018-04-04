#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "utils.hh"

#include <string>
#include <map>
#include <vector>
#include <optional>
#include <functional>

#define BLOCK_SIZE 32
#define FREEZE 0.75
#define ACTION 0.1

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

class ScoringSystem {

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

struct Timer {
	sf::Time duration, remaining;
	bool flag = false;

	Timer(sf::Time duration):
		duration(duration), remaining(sf::seconds(0)) {};

	bool timeout() {
		return remaining <= sf::seconds(0);
	}

	void tick(sf::Time elapsed) {
		remaining -= elapsed;
	}

	void reset() {
		remaining = duration;
		flag = false;
	}
};

struct State {
	Timer gravity = Timer(sf::seconds(0.8));
	Timer freeze = Timer(sf::seconds(FREEZE));
	Timer action = Timer(sf::seconds(ACTION));

	bool game_over = false;
	int level = 1;
	int score = 0;

	sf::Time turn_length(int level) {
		return sf::seconds(0.8);
	}

	std::vector<Timer*> timers() {
		return {&gravity, &freeze, &action};
	}

	void update(sf::Time elapsed) {
		for (auto timer : timers())
			if (timer->timeout())
				timer->reset();
			else
				timer->tick(elapsed);
	}
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
		BlockMatrix data;

		Matrix(TetrominoType type, BlockFactory &factory):
			factory(factory), type(type) {};

		void rotate_right();
		void rotate_left();
		void draw(Window &window, Vec2 offset = Vec2(0, 0));
		Collision collision(Matrix &board);
		Vec2 get_size();
		void set_size(Vec2 size);
		void update();
		bool valid_point(Vec2 point);
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

		void draw(Window &window, bool shadow);
		void freeze();
		void spawn();
		void hard_drop();
		bool can_move_right(Matrix m);
		bool can_move_left(Matrix m);
		bool can_move_down(Matrix m);
		bool can_rotate_right(Matrix m);
		bool game_over();
		std::vector<int> full_lines();
		void clear_row(int row);
};

#endif

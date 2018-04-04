#include "game.hh"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

// TODO: handle errors in user input

///////////////////////////////////////////////////////////////////////////////
// RotationSystem
///////////////////////////////////////////////////////////////////////////////

RotationSystem::RotationSystem(std::string file_path) {
	using namespace std;
	ifstream data(file_path);

	for (auto type : tetromino_types) {
		int size = (type == I || type == O) ? 4 : 3;
		TetrominoRotation rotation;

		for (int n = 0; n < 4; n++) {
			TetrominoShape shape;
			shape.resize(size);

			for (int y = 0; y < size; y++) {
				shape[y].resize(size);

				for (int x = 0; x < size; x++) {
					int byte; data >> byte;
					shape[y][x] = byte;
				}
			}

			rotation[n] = shape;
		}

		auto ttype = static_cast<TetrominoType>(type);
		this->data[ttype] = rotation;
	}
	cerr << "[done] reading rotation system\n";
}

TetrominoShape RotationSystem::get_shape(TetrominoType type, int rotation) {
	return this->data[type][rotation % 4];
}

///////////////////////////////////////////////////////////////////////////////
// ColorScheme
///////////////////////////////////////////////////////////////////////////////

Color color_from_hex(std::string s) {
	if (s[0] == '0' && s[1] == 'x')
		s.erase(0, 2);
	if (s[0] == '#')
		s.erase(0, 1);
	int hex;
	sscanf(s.c_str(), "%x", &hex);
	return Color(hex);
}

ColorScheme::ColorScheme(std::string file_path) {
	using namespace std;
	ifstream data(file_path);
	for (size_t i = 0; i < tetromino_types.size(); i++) {
		string type, hex;
		data >> type >> hex;
		auto ttype = static_cast<TetrominoType>(type[0]);
		this->data[ttype] = color_from_hex(hex);
	}
	cerr << "[done] reading color scheme\n";
}

Color ColorScheme::get_color(TetrominoType type) {
	return this->data[type];
}

///////////////////////////////////////////////////////////////////////////////
// BlockFactory
///////////////////////////////////////////////////////////////////////////////

BlockMatrix BlockFactory::get_blocks(TetrominoType type, int rotation) {
	auto shape = this->rotation_system.get_shape(type, rotation);
	// FIXME: color schemes don't work
	//auto color = this->color_scheme.get_color(type);
	auto color = Color(random(0, 255), random(0, 255), random(0, 255));

	BlockMatrix blocks;
	blocks.resize(shape.size());
	for (size_t y = 0; y < shape.size(); y++) {
		blocks[y].resize(shape[y].size());
		for (size_t x = 0; x < shape[y].size(); x++) {
			if (shape[y][x]) {
				blocks[y][x] = Block(color);
			} else {
				blocks[y][x] = std::nullopt;
			}
		}
	}

	return blocks;
}

///////////////////////////////////////////////////////////////////////////////
// Matrix
///////////////////////////////////////////////////////////////////////////////

void Matrix::set_size(Vec2 size) {
	this->data.resize(size.y);
	for (int y = 0; y < size.y; y++) {
		this->data[y].resize(size.x);
		for (int x = 0; x < size.x; x++) {
			this->data[y][x] = std::nullopt;
		}
	}
}

Vec2 Matrix::get_size() {
	return Vec2(this->get_width(), this->get_height());
}

int Matrix::get_width() {
	return this->data[0].size();
}

int Matrix::get_height() {
	return this->data.size();
}

void Matrix::rotate_right() {
	this->rotation += 1;
	this->rotation %= 4;
	this->update();
}

void Matrix::rotate_left() {
	this->rotation = this->rotation > 0 ? this->rotation - 1 : 3;
	this->update();
}

void Matrix::update() {
	this->data = this->factory.get_blocks(this->type, this->rotation);
}

void Matrix::draw(Window &window) {
	for (int y = 0; y < this->get_size().y; y++) {
		for (int x = 0; x < this->get_size().x; x++) {
			auto block = this->data[y][x];
			if (block.has_value()) {
				sf::RectangleShape rect(to_f(this->block_size));
				auto pos = mul(add(this->origin, Vec2(x, y)), this->block_size);
				auto color = block->color;
				rect.setFillColor(color);
				rect.setPosition(to_f(pos));
				window.draw(rect);
			}
		}
	}
}

bool Matrix::valid_point(Vec2 point) {
	return point.x >= this->origin.x && point.x < this->origin.x + this->get_width()
		&& point.y >= this->origin.y && point.y < this->origin.y + this->get_height();
}

Collision Matrix::collision(Matrix &board) {
	Collision res;
	for (int y = 0; y < this->get_height(); y++) {
		for (int x = 0; x < this->get_width(); x++) {
			int Y = this->origin.y + board.origin.y + y;
			int X = this->origin.x + board.origin.x + x;
			if (this->data[y][x].has_value()) {
				if (board.valid_point(Vec2(X, Y)) && board.data[Y][X].has_value())
					res.intersect = true;
				if (X < board.origin.x)
					res.left = true;
				if (X >= board.get_width())
					res.right = true;
				if (Y < board.origin.y)
					res.top = true;
				if (Y >= board.get_height())
					res.bottom = true;
			}
		}
	}
	return res;
}

bool Collision::any() {
	return this->left || this->right || this->top || this->bottom || this->intersect;
}

///////////////////////////////////////////////////////////////////////////////
// Board
///////////////////////////////////////////////////////////////////////////////

void Board::draw(Window &window) {
	sf::RectangleShape rect(to_f(mul(this->board.get_size(), this->block_size)));
	// TODO: center the board
	rect.setPosition(Vec2f(0,0));
	window.draw(rect);

	this->board.draw(window);
	this->falling.draw(window);
}

void Board::insert(Matrix matrix) {
	for (int y = 0; y < matrix.get_size().y; y++) {
		for (int x = 0; x < matrix.get_size().x; x++) {
			int Y = matrix.origin.y + this->board.origin.y + y;
			int X = matrix.origin.x + this->board.origin.x + x;
			if (matrix.data[y][x].has_value() && this->board.valid_point(Vec2(X, Y))) {
				this->board.data[Y][X] = matrix.data[y][x];
			}
		}
	}
}

bool Board::can_move_right() {
	Matrix future = this->falling;
	future.origin.x += 1;
	auto col = future.collision(this->board);
	return !(col.right || col.intersect);
}

bool Board::can_move_left() {
	Matrix future = this->falling;
	future.origin.x -= 1;
	auto col = future.collision(this->board);
	return !(col.left || col.intersect);
}

bool Board::can_rotate_right() {
	Matrix future = this->falling;
	future.rotate_right();
	auto col = future.collision(this->board);
	return !col.any();
}

bool Board::can_move_down() {
	Matrix future = this->falling;
	future.origin.y += 1;
	auto col = future.collision(this->board);
	return !(col.bottom || col.intersect);
}

bool Board::should_freeze() {
	return !this->can_move_down();
}

bool Board::game_over() {
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < this->board.get_size().x; x++) {
			if (this->board.data[y][x].has_value()) {
				return true;
			}
		}
	}
	return false;
}

void Board::freeze() {
	this->insert(this->falling);
}

void Board::spawn() {
	this->falling = Matrix(this->tetromino_factory.next(), this->block_factory);
	this->falling.update();
	this->falling.origin = Vec2(3, -1);
}

//
// Tetromino Factories
//

TetrominoType RandomTetrominoFactory::next() {
	auto index = random(0, tetromino_types.size() - 1);
	auto type = tetromino_types[index];
	return static_cast<TetrominoType>(type);
}

TetrominoType BaggedTetrominoFactory::next() {
	if (this->bag.empty()) {
		this->bag = tetromino_types;
		std::shuffle(this->bag.begin(), this->bag.end(), random_generator());
	}
	auto type = this->bag.back();
	this->bag.pop_back();
	return static_cast<TetrominoType>(type);
}


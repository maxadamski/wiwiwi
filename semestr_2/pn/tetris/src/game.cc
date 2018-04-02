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

Matrix::Matrix(TetrominoType type, Vec2 origin, Vec2 size, Vec2 block_size) {
	this->origin = origin;
	this->block_size = block_size;
	this->rotation = 0;
	this->type = type;

	this->data.resize(size.y);
	for (int y = 0; y < size.y; y++) {
		this->data[y].resize(size.x);
		for (int x = 0; x < size.x; x++) {
			this->data[y][x] = std::nullopt;
		}
	}
}

void Matrix::rotate_right() {
	this->rotation += 1;
	this->rotation %= 4;
}

void Matrix::rotate_left() {
	this->rotation = this->rotation > 0 ? this->rotation - 1 : 3;
}

void Matrix::update_tetromino(BlockFactory &factory) {
	this->data = factory.get_blocks(this->type, this->rotation);
}

void Matrix::draw(Window &window) {
	for (int y = 0; y < this->get_size().y; y++) {
		for (int x = 0; x < this->get_size().x; x++) {
			auto block = this->data[y][x];
			if (block.has_value()) {
				sf::RectangleShape rect(to_f(this->block_size));
				auto pos = mul(add(this->origin, Vec2(x, y)), this->block_size);
				rect.setFillColor(block->color);
				rect.setPosition(to_f(pos));
				window.draw(rect);
			}
		}
	}
}

Vec2 Matrix::get_size() {
	return Vec2(this->data[0].size(), this->data.size());
}

bool Matrix::collides(Matrix &parent) {
	for (int y = 0; y < this->get_size().y; y++) {
		for (int x = 0; x < this->get_size().x; x++) {
			int x_ = this->origin.x + x;
			int y_ = this->origin.y + y;
			int w = parent.get_size().x;
			int h = parent.get_size().y;
			if (this->data[y][x] && x_ >= 0 && y_ >= 0 && x_ < w && y_ < h && parent.data[y_][x_]) {
				return true;
			}
		}
	}
	return false;
}

bool Matrix::outside(Matrix &parent) {
	for (int y = 0; y < this->get_size().y; y++) {
		for (int x = 0; x < this->get_size().x; x++) {
			int x_ = this->origin.x + x;
			int y_ = this->origin.y + y;
			int w = parent.get_size().x;
			int h = parent.get_size().y;
			if (this->data[y][x] && (x_ < 0 || y_ < 0 || x_ >= w || y_ >= h)) {
				return true;
			}
		}
	}
	return false;
}

bool Matrix::outside_y(Matrix &parent) {
	for (int y = 0; y < this->get_size().y; y++) {
		for (int x = 0; x < this->get_size().x; x++) {
			int x_ = this->origin.x + x;
			int y_ = this->origin.y + y;
			int w = parent.get_size().x;
			int h = parent.get_size().y;
			if (this->data[y][x] && (y_ < 0 || y_ >= h)) {
				return true;
			}
		}
	}
	return false;
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
	if (this->falling.has_value()) {
		this->falling->draw(window);
	}
}

void Board::insert(Matrix matrix) {
	for (int y = 0; y < matrix.get_size().y; y++) {
		for (int x = 0; x < matrix.get_size().x; x++) {
			if (matrix.data[y][x].has_value()) {
				int y_ = matrix.origin.y + y;
				int x_ = matrix.origin.x + x;
				this->board.data[y_][x_] = matrix.data[y][x];
			}
		}
	}
}

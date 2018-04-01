#include "game.hh"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

const std::vector<char> tetromino_types = {I, O, J, L, S, T, Z};

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

Matrix::Matrix(Vec2 origin, Vec2 size, Vec2 block_size) {
	this->origin = origin;
	this->size = size;
	this->block_size = block_size;
	this->rotation = 0;

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

void Matrix::update_tetromino(TetrominoType type, BlockFactory &factory) {
	this->data = factory.get_blocks(type, this->rotation);
}

void Matrix::draw(Window &window) {
	for (size_t y = 0; y < this->data.size(); y++) {
		for (size_t x = 0; x < this->data[y].size(); x++) {
			auto block = this->data[y][x];
			if (block.has_value()) {
				sf::RectangleShape rect(to_f(this->block_size));
				auto pos = add(this->origin, mul(Vec2(x, y), this->block_size));
				rect.setFillColor(block->color);
				rect.setPosition(to_f(pos));
				window.draw(rect);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Board
///////////////////////////////////////////////////////////////////////////////

void Board::draw(Window &window) {
	sf::RectangleShape rect(to_f(mul(this->size, this->block_size)));
	rect.setPosition(Vec2f(0,0));
	window.draw(rect);

	this->board.draw(window);
}

void Board::insert(Matrix matrix, Vec2 origin) {
	
}

// Overrides blocks in matrix with given tetromino
//void Matrix::place(Tetromino tetromino, Vec2 point) {
//	auto system = this->rotation_system;
//	auto shape = system[tetromino.type][tetromino.rotation];
//	auto shape_size = (Vec2) { 4, 4 };
//	Block block;
//	block.type = type;
//	block.tetromino_type = tetromino->type;
//	for (int y = 0; y < shape_size.x; y += 1) {
//		for (int x = 0; x < shape_size.y; x += 1) {
//			if (shape[y][x] == 1) {
//				this->blocks[point.y + y][point.x + x] = block;
//			}
//		}
//	}
//}
//
//void Matrix::spawn(Tetromino tetromino) {
//	matrix_override(matrix, tetromino, {4, 0}, BLOCK_FALLING);
//}

//void Board::draw(Window window) {
//	Vec2 frame_size(
//		(MATRIX_W + 2) * block_size.x,
//		(MATRIX_H + 2) * block_size.y
//	);
//	Vec2 frame_origin(
//		display_center.x - frame_size.x / 2,
//		display_center.y - frame_size.y / 2
//	);
//
//	auto buffer_size = (Vec2) {frame_size.x, block_edge*2};
//	draw_rect_frame({frame_origin.x, frame_origin.y + block_edge*2}, 
//			buffer_size, WHITE, 2);
//	draw_rect_frame(frame_origin, frame_size, WHITE, 1);
//	draw_rect_frame(frame_origin, buffer_size, RED, 2);
//
//	for (int y = 0; y < MATRIX_H; y += 1) {
//		for (int x = 0; x < MATRIX_W; x += 1) {
//			Block block = matrix->blocks[y][x];
//
//			if (block.type != BLOCK_NONE) {
//				auto block_color = matrix->color_theme[block.tetromino_type];
//				Vec2 block_origin(
//					frame_origin.x + block_size.x * x,
//					frame_origin.y + block_size.y * y
//				);
//				draw_rect_fill(block_origin, block_size, block_color);
//			}
//		}
//	}
//}

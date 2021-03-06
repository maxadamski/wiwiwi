#include "game.hh"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// TODO: handle errors in user input

///////////////////////////////////////////////////////////////////////////////
// RotationSystem
///////////////////////////////////////////////////////////////////////////////

RotationSystem::RotationSystem(std::string file_path) {
	using namespace std;
	ifstream input(file_path);

	for (auto type : tetromino_types) {
		int size = (type == I || type == O) ? 4 : 3;
		TetrominoRotation rotation;

		for (int n = 0; n < 4; n++) {
			TetrominoShape shape;
			shape.resize(size);

			for (int y = 0; y < size; y++) {
				shape[y].resize(size);

				for (int x = 0; x < size; x++) {
					int byte; input >> byte;
					shape[y][x] = byte;
				}
			}

			rotation[n] = shape;
		}

		auto ttype = static_cast<TetrominoType>(type);
		data[ttype] = rotation;
	}
	cerr << "[done] reading rotation system\n";
}

TetrominoShape RotationSystem::get_shape(TetrominoType type, int rotation) {
	return data[type][rotation % 4];
}

///////////////////////////////////////////////////////////////////////////////
// ColorScheme
///////////////////////////////////////////////////////////////////////////////

ColorScheme::ColorScheme(std::string file_path) {
	std::ifstream input(file_path);
	for (size_t i = 0; i < tetromino_types.size(); i++) {
		std::string type; unsigned int color;
		input >> type >> std::hex >> color;
		color = color << 8 | 0xFF;
		auto tetromino_type = static_cast<TetrominoType>(type[0]);
		data[tetromino_type] = Color(color);
	}
	std::cerr << "[done] reading color scheme\n";
}

Color ColorScheme::get_color(TetrominoType type) {
	return data[type];
}

///////////////////////////////////////////////////////////////////////////////
// BlockFactory
///////////////////////////////////////////////////////////////////////////////

BlockMatrix BlockFactory::get_blocks(TetrominoType type, int rotation) {
	auto shape = rotation_system.get_shape(type, rotation);
	auto color = color_scheme.get_color(type);

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
// Block
///////////////////////////////////////////////////////////////////////////////

void Block::draw(Window &window, Vec2 origin, Vec2 size) {
	sf::RectangleShape rect(to_f(size));
	rect.setPosition(to_f(origin));

	int padding = 10;
	Vec2 front_size = add(size, Vec2(-2*padding, -2*padding));
	Vec2 front_pos = add(origin, Vec2(padding, padding));
	sf::RectangleShape front(to_f(front_size));
	front.setPosition(to_f(front_pos));

	if (shadow) {
		rect.setFillColor(Color(0, 0, 0, 255*0.2));
		window.draw(rect);
	} else {
		front.setFillColor(Color(0, 0, 0, 255*0.15));
		rect.setFillColor(color);
		window.draw(rect);
		window.draw(front);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Matrix
///////////////////////////////////////////////////////////////////////////////

void Matrix::set_size(Vec2 size) {
	data.resize(size.y);
	for (int y = 0; y < size.y; y++) {
		data[y].resize(size.x);
		for (int x = 0; x < size.x; x++) {
			data[y][x] = std::nullopt;
		}
	}
}

Vec2 Matrix::get_size() {
	return Vec2(get_width(), get_height());
}

int Matrix::get_width() {
	return data[0].size();
}

int Matrix::get_height() {
	return data.size();
}

bool Matrix::was_on_hold() {
	return hold_count > 0;
}

void Matrix::rotate_right() {
	rotation += 1;
	rotation %= 4;
	update();
}

void Matrix::rotate_left() {
	rotation = rotation > 0 ? rotation - 1 : 3;
	update();
}

void Matrix::rotate_reset() {
	rotation = 0;
	update();
}

void Matrix::update() {
	data = factory.get_blocks(type, rotation);
}

void Matrix::draw(Window &window, Vec2 offset) {
	for (int y = 0; y < get_size().y; y++) {
		for (int x = 0; x < get_size().x; x++) {
			auto point = mul(add(origin, Vec2(x, y)), block_size);
			auto block = data[y][x];
			if (block.has_value()) {
				block->draw(window, add(offset, point), block_size);
			}
		}
	}
}

bool Matrix::valid_point(Vec2 point) {
	return point.x >= origin.x && point.x < origin.x + get_width()
		&& point.y >= origin.y && point.y < origin.y + get_height();
}

Collision Matrix::collision(Matrix &board) {
	Collision res;
	for (int y = 0; y < get_height(); y++) {
		for (int x = 0; x < get_width(); x++) {
			int Y = origin.y + board.origin.y + y;
			int X = origin.x + board.origin.x + x;
			if (data[y][x].has_value()) {
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
	return left || right || top || bottom || intersect;
}

///////////////////////////////////////////////////////////////////////////////
// Board
///////////////////////////////////////////////////////////////////////////////

void Board::draw(Window &window, Vec2 offset, bool shadow) {
	sf::RectangleShape rect(to_f(mul(board.get_size(), block_size)));
	// TODO: center the board
	rect.setFillColor(Color(BOARD_COLOR));
	rect.setPosition(to_f(offset));
	window.draw(rect);
	board.draw(window, offset);
	if (shadow) {
		Matrix s = falling;
		// hard drop the shadow
		while (can_move_down(s))
			s.origin.y += 1;
		// recolor it
		for (int y = 0; y < s.get_size().y; y++) {
			for (int x = 0; x < s.get_size().x; x++) {
				if (s.data[y][x].has_value()) {
					s.data[y][x]->shadow = true;
				}
			}
		}
		s.draw(window, offset);
	};
	falling.draw(window, offset);
}

void Board::insert(Matrix matrix) {
	for (int y = 0; y < matrix.get_size().y; y++) {
		for (int x = 0; x < matrix.get_size().x; x++) {
			int Y = matrix.origin.y + board.origin.y + y;
			int X = matrix.origin.x + board.origin.x + x;
			if (matrix.data[y][x].has_value() && board.valid_point(Vec2(X, Y))) {
				board.data[Y][X] = matrix.data[y][x];
			}
		}
	}
}

bool Board::can_move_right(Matrix m) {
	m.origin.x += 1;
	auto col = m.collision(board);
	return !(col.right || col.intersect);
}

bool Board::can_move_left(Matrix m) {
	m.origin.x -= 1;
	auto col = m.collision(board);
	return !(col.left || col.intersect);
}

bool Board::can_move_down(Matrix m) {
	m.origin.y += 1;
	auto col = m.collision(board);
	return !(col.bottom || col.intersect);
}

bool Board::can_rotate_right(Matrix m) {
	m.rotate_right();
	auto col = m.collision(board);
	return !col.any();
}

bool Board::can_rotate_left(Matrix m) {
	m.rotate_left();
	auto col = m.collision(board);
	return !col.any();
}

bool Board::game_over() {
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < board.get_size().x; x++) {
			if (board.data[y][x].has_value()) {
				return true;
			}
		}
	}
	return false;
}

void Board::freeze() {
	insert(falling);
}

void Board::spawn() {
	falling = next;
	falling.update();
	next = Matrix(tetromino_factory.next(), block_factory);
	next.update();
	falling.origin = SPAWN_POS;
}

void Board::hold_swap() {
	Matrix temp = falling;
	temp.origin = Vec2(0, 0);
	temp.rotate_reset();
	temp.hold_count += 1;

	if (hold)  {
		falling = hold.value();
		falling.origin = SPAWN_POS;
	} else {
		spawn();
	}

	hold = temp;
}

void Board::hard_drop() {
	while (can_move_down(falling))
		falling.origin.y += 1;
}

std::vector<int> Board::full_lines() {
	std::vector<int> lines;
	for (int y = 0; y < board.get_size().y; y++) {
		bool full = true;
		for (int x = 0; full && x < board.get_size().x; x++)
			if (!board.data[y][x].has_value()) full = false;
		if (full) lines.push_back(y);
	}
	return lines;
}

void Board::clear_row(int row) {
	for (int x = 0; x < board.get_size().x; x++)
		board.data[row][x] = std::nullopt;

	for (int y = row; y > 0; y--)
		for (int x = 0; x < board.get_size().x; x++)
			board.data[y][x] = board.data[y-1][x];
}

///////////////////////////////////////////////////////////////////////////////
// TetrominoFactory
///////////////////////////////////////////////////////////////////////////////

TetrominoType RandomTetrominoFactory::next() {
	auto index = random(0, tetromino_types.size() - 1);
	auto type = tetromino_types[index];
	return static_cast<TetrominoType>(type);
}

TetrominoType BaggedTetrominoFactory::next() {
	if (bag.empty()) {
		bag = tetromino_types;
		do {
			std::shuffle(bag.begin(), bag.end(), random_generator());
		} while (last && last.value() == bag.back());
	}
	auto type = bag.back();
	last = type;
	bag.pop_back();
	return static_cast<TetrominoType>(type);
}

///////////////////////////////////////////////////////////////////////////////
// TetrominoFactory
///////////////////////////////////////////////////////////////////////////////

int State::level() {
	return lines / 10;
}

double State::turn_frames() {
	int L = level();
	if (L <= 8)
		return 48 - L*5;
	else
		return max(6 - (L-9)*0.25, 1);
}

sf::Time State::turn_length() {
	return sf::seconds(turn_frames() / 60.0);
}

std::vector<Timer*> State::timers() {
	return {&gravity, &freeze, &action};
}

void State::update_gravity() {
	gravity = Timer(turn_length());
}

int State::elapsed_seconds() {
	return elapsed_time.asSeconds();
}

void State::update(sf::Time elapsed) {
	if (!game_over)
		elapsed_time += elapsed;
	for (auto timer : timers())
		if (timer->timeout())
			timer->reset();
		else
			timer->tick(elapsed);
}

void State::write_report(std::string path) {
	std::ofstream report(path, std::ios::app);
	report << "---\n";
	report << "score " << score << "\n";
	report << "time " << elapsed_seconds() << "\n";
	report << "level " << level() << "\n";
	report << "lines " << lines << "\n";
	std::cerr << "[done] writing report\n";
}


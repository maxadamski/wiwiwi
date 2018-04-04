#include "main.hh"
#include <iostream>
#include <algorithm>

#define SHADOW true

struct Input {
	bool move_right = false,
		 move_left = false,
		 rotate_left = false,
		 rotate_right = false,
		 hold = false,
		 soft_drop = false,
		 hard_drop = false,
		 sonic_drop = false;
};

Input get_input(Window &window) {
	Input input;

	for (sf::Event event; window.pollEvent(event);) {
		switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::KeyPressed:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				input.rotate_right = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				input.move_right = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				input.move_left = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				input.hard_drop = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				input.soft_drop = true;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				input.hold = true;
			break;

		default:
			break;
		}
	}

	return input;
}

sf::Font kouryuu;
#include <cstdio>

void render(Window &window, Board &board, State &state) {
	window.clear();
	board.draw(window, SHADOW);

	std::string score_string;
	sprintf(&score_string[0], "%08d", state.score);
	sf::Text score(score_string, kouryuu, 32);
	score.setFillColor(Color(255,255,255));
	score.setPosition(to_f(Vec2(300, 100)));
	window.draw(score);

	std::string level;
	sprintf(&level[0], "%02d", state.level);
	sf::Text level_label("level", kouryuu, 32);
	sf::Text level_value(level, kouryuu, 32);
	window.draw(level_label);
	window.draw(level_value);
	
	window.display();
}

void update(Board &board, Input &input, State &state) {
	int dx = 0, dy = 0, rotate = false;

	if (state.gravity.timeout() && board.can_move_down(board.falling))
		dy = 1;

	if (!state.action.flag) {
		if (input.soft_drop && board.can_move_down(board.falling))
			dy = 1;

		if (input.move_right && board.can_move_right(board.falling))
			dx = 1;

		if (input.move_left && board.can_move_left(board.falling))
			dx = -1;

		if (input.rotate_right && board.can_rotate_right(board.falling))
			rotate = true;
	}

	if (dx != 0 || rotate) {
		state.action.flag = true;
	}

	// too bad so sad
	if (board.game_over()) {
		std::cerr << "[event] game over\n";
		state.game_over = true;
		return;
	}

	if (input.hard_drop) {
		std::cerr << "[event] hard drop\n";
		board.hard_drop();
	} else {
		board.falling.origin.y += dy;

		if (rotate || dx != 0) state.freeze.reset();

		if (rotate) {
			board.falling.rotate_right();
		} else {
			board.falling.origin.x += dx;
		}
	}

	auto full_rows = board.full_lines();
	if (!full_rows.empty()) {
		for (auto row : full_rows) board.clear_row(row);
	}

	if (!board.can_move_down(board.falling) 
			&& (state.freeze.timeout() || input.soft_drop || input.hard_drop)) {
		std::cerr << "[event] freeze\n";
		board.freeze();
		board.spawn();
	}
}

int main() {
	random_seed();
	RotationSystem rotation_system(ROTATION_SYSTEM_PATH);
	ColorScheme color_scheme(COLOR_SCHEME_PATH);
	BlockFactory bfactory(rotation_system, color_scheme);
	BaggedTetrominoFactory tfactory;
	Board board(Vec2(BOARD_W, BOARD_H), tfactory, bfactory);
	board.spawn();
	kouryuu.loadFromFile("res/font/kouryuu.ttf");

	sf::RenderWindow window(sf::VideoMode(1280, 960), "sfml-devel");
	window.setVerticalSyncEnabled(true);
	sf::Clock clock;
	State state;

	auto spu = sf::seconds(1.0 / 60);
	while (window.isOpen()) {
		auto start = clock.getElapsedTime();
		auto input = get_input(window);
		state.update(spu);
		update(board, input, state);
		render(window, board, state);
		if (state.game_over) break;
		sf::sleep(start + spu - clock.restart());
	}

	std::cerr << "[event] exiting gracefully\n";
	return 0;
}


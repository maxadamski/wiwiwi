#include "main.hh"
#include <iostream>
#include <algorithm>

typedef struct Input Input;
struct Input {
	bool move_right, move_left, rotate_left, rotate_right, 
		 hold, soft_drop, hard_drop;

	Input() {
		rotate_right = false;
		rotate_left = false;
		move_right = false;
		move_left = false;
		soft_drop = false;
		hard_drop = false;
		hold = false;
	}
};

typedef struct State State;
struct State {
	sf::Time turn = sf::seconds(0);
	bool game_over = false;
	int level = 1;

	bool should_apply_gravity() {
		return turn >= turn_length(this->level);
	}

	void update_turn(sf::Time elapsed) {
		if (turn >= turn_length(this->level)) {
			turn -= turn_length(this->level);
		}
		turn += elapsed;
	}

	sf::Time turn_length(int level) {
		return sf::seconds(0.8);
	}
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

void render(Window &window, Board &board, State &state) {
	window.clear();
	board.draw(window);
	window.display();
}

void update(Board &board, Input &input, State &state) {
	int dx = 0, dy = 0, rotate = false;
	if ((input.soft_drop || state.should_apply_gravity()) && board.can_move_down())
		dy = 1;
	if (input.move_right && board.can_move_right())
		dx = 1;
	if (input.move_left && board.can_move_left())
		dx = -1;
	if (input.rotate_right && board.can_rotate_right())
		rotate = true;

	// too bad so sad
	if (board.is_game_over()) {
		std::cerr << "game over\n";
		state.game_over = true;
		return;
	}

	board.falling.origin.y += dy;
	if (rotate) {
		board.falling.rotate_right();
	} else {
		board.falling.origin.x += dx;
	}


	// freeze tetromino
	if (dy > 0 && board.should_freeze()) {
		std::cerr << "freezing\n";
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

	sf::RenderWindow window(sf::VideoMode(1280, 960), "sfml-devel");
	window.setVerticalSyncEnabled(true);
	sf::Clock clock;
	State state;

	auto spu = sf::seconds(1.0 / 50);
	while (window.isOpen()) {
		auto start = clock.getElapsedTime();
		auto input = get_input(window);
		update(board, input, state);
		render(window, board, state);
		if (state.game_over) break;
		state.update_turn(spu);
		sf::sleep(start + spu - clock.restart());
	}


	std::cerr << "[event] exiting gracefully\n";
	return 0;
}


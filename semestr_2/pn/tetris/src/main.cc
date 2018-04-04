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
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
				input.rotate_left = true;
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
#include <iomanip>

void render(Window &window, Board &board, State &state) {
	window.clear();
	board.draw(window, SHADOW);

	std::ostringstream score;
	score << std::setfill('0') << std::setw(8) << state.score;

	sf::Text score_label("Score", kouryuu, 32);
	score_label.setPosition(to_f(Vec2(400, 100)));
	window.draw(score_label);

	sf::Text score_value(score.str(), kouryuu, 32);
	score_value.setPosition(to_f(Vec2(400, 140)));
	window.draw(score_value);


	std::ostringstream level;
	level << std::setfill('0') << std::setw(8) << state.level();

	sf::Text level_label("Level", kouryuu, 32);
	level_label.setPosition(to_f(Vec2(400, 200)));
	window.draw(level_label);

	sf::Text level_value(level.str(), kouryuu, 32);
	level_value.setPosition(to_f(Vec2(400, 240)));
	window.draw(level_value);
	

	sf::Text next_label("Next", kouryuu, 32);
	next_label.setPosition(to_f(Vec2(400, 300)));
	window.draw(next_label);

	board.next.draw(window, Vec2(400, 320));

	sf::Text hold_label("Hold", kouryuu, 32);
	hold_label.setPosition(to_f(Vec2(400, 450)));
	window.draw(hold_label);

	if (board.hold)
		board.hold->draw(window, Vec2(400, 470));

	window.display();
}

int score_for_lines(int lines, int level) {
	if (lines <= 0 || lines > 4) return 0;
	std::vector<int> mul = {40, 100, 300, 1200};
	return mul[lines - 1] * (level + 1);
}

void update(Board &board, Input &input, State &state) {
	int dx = 0, dy = 0, rotate = 0;

	if (input.hold && !board.falling.was_on_hold())
		board.hold_swap();

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
			rotate = 1;
		if (input.rotate_left && board.can_rotate_left(board.falling))
			rotate = -1;
	}

	if (dx != 0 || rotate != 0) {
		state.action.flag = true;
	}

	// too bad so sad
	if (board.game_over()) {
		std::cerr << "[event] game over\n";
		state.game_over = true;
		return;
	}

	if (input.hard_drop) {
		board.hard_drop();
	} else {
		board.falling.origin.y += dy;

		if (rotate != 0 || dx != 0)
			state.freeze.reset();

		if (rotate != 0) {
			board.falling.rotate_right();
		} else {
			board.falling.origin.x += dx;
		}
	}

	auto full_rows = board.full_lines();
	auto level_before = state.level();
	state.lines += full_rows.size();
	state.score += score_for_lines(full_rows.size(), state.level());
	if (state.level() > level_before)
		state.update_gravity();
	for (auto row : full_rows) 
		board.clear_row(row);

	if (!board.can_move_down(board.falling) 
			&& (state.freeze.timeout() || input.soft_drop 
				|| (!SONIC_DROP && input.hard_drop))) {
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

	State state;
	state.update_gravity();

	sf::RenderWindow window(sf::VideoMode(960, 960), "sfml-devel");
	window.setVerticalSyncEnabled(true);
	sf::Clock clock;

	kouryuu.loadFromFile(FONT_KOURYUU_PATH);

	sf::Music music_a;
	if (!music_a.openFromFile(MUSIC_A_PATH)) {
		std::cerr << "[error] coldn't load " << MUSIC_A_PATH << "\n";
		return 1;
	}
	music_a.setLoop(true);
	music_a.setVolume(50);
	music_a.play();

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


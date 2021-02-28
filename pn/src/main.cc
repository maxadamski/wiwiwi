#include "main.hh"
#include <iostream>
#include <iomanip>
#include <fstream>

sf::Font kouryuu;


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

Vec2 draw_label(Window &w, std::string text, Vec2 position,
		sf::Font font = kouryuu, int font_size = 32) {
	sf::Text label(text, font, font_size);
	label.setPosition(to_f(position));
	label.setFillColor(Color(FONT_COLOR));
	w.draw(label);
	return position;
}

void render(Window &window, Board &board, State &state) {
	Window &w = window;

	auto font = kouryuu;
	int window_w = WINDOW_W, window_h = WINDOW_H;
	int font_s = 32;
	int line_h = font_s + 8;
	int section_pad = 20;
	int bar_w = 5*BLOCK_SIZE;
	int tetron_w = BOARD_W * BLOCK_SIZE, tetron_h = BOARD_H * BLOCK_SIZE;
	int tetron_x = (window_w - tetron_w - bar_w - section_pad) / 2 - 10;
	int tetron_y = (window_h - tetron_h) / 2;
	int bar_x = tetron_x + tetron_w + section_pad;
	int bar_y = tetron_y;
	int tetromino_h = 3*BLOCK_SIZE;

	std::ostringstream level, score, time, lines;
	level << std::setfill('0') << std::setw(8) << state.level();
	score << std::setfill('0') << std::setw(8) << state.score;
	time  << std::setfill('0') << std::setw(8) << state.elapsed_seconds();
	lines << std::setfill('0') << std::setw(8) << state.lines;

	window.clear(Color(BG_COLOR));

	board.draw(window, Vec2(tetron_x, tetron_y), SHADOW);


	sf::Text title("Max's Totally Amazing Tetris Clone", font, font_s);
	title.setPosition(to_f(Vec2((window_w - title.getLocalBounds().width) / 2, 45)));
	title.setFillColor(Color(FONT_COLOR));
	w.draw(title);

	if (state.game_over) {
		sf::Text gover("Game Over!", font, font_s);
		gover.setPosition(to_f(Vec2((window_w - gover.getLocalBounds().width) / 2, tetron_y + tetron_h + 45)));
		gover.setFillColor(Color(RED_COLOR));
		w.draw(gover);
	}

	Vec2 time_section = draw_label(w, "Time", 
		Vec2(bar_x, bar_y));
	Vec2 time_value = draw_label(w, time.str(), 
		Vec2(bar_x, time_section.y + line_h));

	Vec2 line_section = draw_label(w, "Lines", 
		Vec2(bar_x, time_value.y + line_h + section_pad));
	Vec2 line_value = draw_label(w, lines.str(), 
		Vec2(bar_x, line_section.y + line_h));

	Vec2 level_section = draw_label(w, "Level", 
		Vec2(bar_x, line_value.y + line_h + section_pad));
	Vec2 level_value = draw_label(w, level.str(), 
		Vec2(bar_x, level_section.y + line_h));

	Vec2 score_label_pos = draw_label(w, "Score",
		Vec2(bar_x, level_value.y + line_h + section_pad));
	Vec2 score_value_pos = draw_label(w, score.str(), 
		Vec2(bar_x, score_label_pos.y + line_h));

	// draw next tetromino piece
	Vec2 next_label_pos = draw_label(w, "Next", 
		Vec2(bar_x, score_value_pos.y + line_h + section_pad));
	Vec2 next_pos = Vec2(bar_x, next_label_pos.y + line_h);
	board.next.draw(window, next_pos);

	Vec2 hold_label_pos = draw_label(w, "Hold", 
		Vec2(bar_x, next_pos.y + tetromino_h + section_pad));
	if (board.hold)
		board.hold->draw(window, Vec2(bar_x, hold_label_pos.y + line_h));

	window.display();
}

int score_for_lines(int lines, int level) {
	if (lines <= 0 || lines > 4) return 0;
	std::vector<int> mul = {40, 100, 300, 1200};
	return mul[lines - 1] * (level + 1);
}

void update(Board &board, Input &input, State &state) {
	if (state.game_over) return;

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
		state.write_report("report.txt");
		return;
	}

	if (input.hard_drop) {
		board.hard_drop();
		state.score += TETROMINO_POINTS;
	} else {
		if (dy != 0 || dx != 0 || rotate != 0)
			state.freeze.reset();

		board.falling.origin.y += dy;

		if (rotate == 1)
			board.falling.rotate_right();
		else if (rotate == -1)
			board.falling.rotate_left();
		else
			board.falling.origin.x += dx;
	}

	if (!board.can_move_down(board.falling) 
			&& (state.freeze.timeout() || input.hard_drop || input.soft_drop)) {
		state.score += TETROMINO_POINTS;
		board.freeze();
		board.spawn();
	}

	auto full_rows = board.full_lines();
	auto level_before = state.level();
	state.lines += full_rows.size();
	state.score += score_for_lines(full_rows.size(), state.level());
	if (state.level() > level_before)
		state.update_gravity();
	for (auto row : full_rows) 
		board.clear_row(row);
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

	sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "sfml-devel");
	window.setVerticalSyncEnabled(true);
	sf::Clock clock;
	kouryuu.loadFromFile(FONT_KOURYUU_PATH);

	auto spu = sf::seconds(1.0 / 60);
	while (window.isOpen()) {
		auto start = clock.getElapsedTime();
		auto input = get_input(window);
		state.update(spu);
		update(board, input, state);
		render(window, board, state);
		sf::sleep(start + spu - clock.restart());
	}

	std::cerr << "[event] exiting gracefully\n";
	return 0;
}


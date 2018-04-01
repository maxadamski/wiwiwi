#include "main.hh"
#include <iostream>

int main() {
	random_seed();
	RotationSystem rotation_system(ROTATION_SYSTEM_PATH);
	ColorScheme color_scheme(COLOR_SCHEME_PATH);
	BlockFactory bfactory(rotation_system, color_scheme);
	Board board(Vec2(BOARD_W, BOARD_H));
	board.falling = Matrix(T, Vec2(3, 0));
	board.falling->update_tetromino(bfactory);
	Matrix m(L, Vec2(2, 20));
	m.rotate_right();
	m.rotate_right();
	m.update_tetromino(bfactory);
	board.insert(m);

	//Matrix oneL(L, Vec2(20, 0));
	//Matrix oneT(T, Vec2(3, 0));
	//oneL.update_tetromino(bfactory);
	//oneT.update_tetromino(bfactory);
	//std::cerr << oneT.collides(oneL) << "\n";
	//

	sf::RenderWindow window(sf::VideoMode(1280, 960), "sfml-devel");
	window.setVerticalSyncEnabled(true);
	sf::Clock clock;

	sf::Font kouryuu;
	kouryuu.loadFromFile(FONT_KOURYUU_PATH);

	double turn_time = 1;
	double turn = 0;

	while (window.isOpen()) {
		sf::Time dt = clock.restart();
		sf::Event event;

		turn += dt.asSeconds();
		bool next_turn = turn >= turn_time;
		if (next_turn) {
			turn -= turn_time;
			std::cerr << "[game] next_turn\n";
		}

		bool rotate = false;
		int dx = 0, dy = 0;

		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					std::cerr << "[event] closed window\n";
					break;

				case sf::Event::KeyPressed:
					if (board.falling.has_value()) {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
							//std::cerr << "[event] press _\n";
							rotate = true;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							//std::cerr << "[event] press <\n";
							dx = +1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							//std::cerr << "[event] press <\n";
							dx = -1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
							//std::cerr << "[event] press v\n";
							dy = +1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
							// TODO: implement hold & remove debug capability
							//std::cerr << "[event] press ^\n";
							dy = -1;
						}
					}
					break;

				default:
					break;
			}
		}

		if (next_turn) {
			dy = +1;
		}

		board.falling->origin.y += dy;
		board.falling->origin.x += dx;
		if (rotate) {
			board.falling->rotate_right();
			board.falling->update_tetromino(bfactory);
			if (board.falling->collides(board.board)) {
				board.falling->rotate_left();
				board.falling->update_tetromino(bfactory);
				rotate = false;
			}
		}

		bool freezed = false;
		if (dy > 0 && (board.falling->collides(board.board) || board.falling->outside_y(board.board))) {
			freezed = true;
			std::cerr << "[event] freezing block\n";
		}
		if (freezed || board.falling->outside(board.board)) {
			board.falling->origin.y -= dy;
			board.falling->origin.x -= dx;
			if (rotate) {
				board.falling->rotate_left();
				board.falling->update_tetromino(bfactory);
			}
		}
		if (freezed) {
			board.insert(board.falling.value());
			int type_index = random(0, tetromino_types.size() - 1);
			auto type = static_cast<TetrominoType>(tetromino_types[type_index]);
			Matrix n(type, Vec2(2, 0));
			n.update_tetromino(bfactory);
			board.falling = n;
		}

		bool cont = board.falling->outside(board.board);
		bool coll = board.falling->collides(board.board);
		//std::cerr << "outside: " << cont << ", collision: " << coll << "\n";

		window.clear();
		board.draw(window);
		window.display();
	}

	std::cerr << "[event] exiting cleanly\n";
	return 0;
}


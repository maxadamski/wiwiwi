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

	while (window.isOpen()) {
		sf::Time dt = clock.restart();
		sf::Event event;

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
							board.falling->rotate_right();
							board.falling->update_tetromino(bfactory);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							//std::cerr << "[event] press <\n";
							board.falling->origin.x += 1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							//std::cerr << "[event] press <\n";
							board.falling->origin.x -= 1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
							//std::cerr << "[event] press v\n";
							board.falling->origin.y += 1;
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
							// TODO: implement hold & remove debug capability
							//std::cerr << "[event] press ^\n";
							board.falling->origin.y -= 1;
						}
					}
					break;

				default:
					break;
			}
		}

		bool cont = board.contains(board.falling.value());
		bool coll = board.board.collides(board.falling.value());
		std::cerr << "cont: " << cont << ", coll: " << coll << "\n";

		window.clear();
		board.draw(window);
		window.display();
	}

	std::cerr << "[event] exiting cleanly\n";
	return 0;
}


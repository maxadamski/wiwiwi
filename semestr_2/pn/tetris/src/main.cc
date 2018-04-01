#include "main.hh"
#include <iostream>

int main() {
	random_seed();
	RotationSystem rotation_system(ROTATION_SYSTEM_PATH);
	ColorScheme color_scheme(COLOR_SCHEME_PATH);
	BlockFactory bfactory(rotation_system, color_scheme);
	Board board(Vec2(0, 0), Vec2(BOARD_W, BOARD_H), Vec2(32, 32));
	Matrix oneL(Vec2(0, 0), Vec2(0, 0), Vec2(32, 32));
	oneL.update_tetromino(L, bfactory);

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
					std::cerr << "[event] pressed key\n";
					break;

				default:
					break;
			}
		}

		window.clear();
		//board.draw(window);
		oneL.draw(window);
		window.display();
	}

	std::cerr << "[event] exiting cleanly\n";
	return 0;
}


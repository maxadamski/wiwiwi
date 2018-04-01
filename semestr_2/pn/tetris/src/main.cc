#include "main.hh"
#include <iostream>

int main() {
	RotationSystem rotation_system(ROTATION_SYSTEM_PATH);
	ColorScheme color_scheme(COLOR_SCHEME_PATH);
	//Board board(Vec2(0, 0), Vec2(BOARD_W, BOARD_H));

	sf::RenderWindow window(sf::VideoMode(800, 600), "sfml-devel");
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
					std::cerr << "closed\n";
					break;

				case sf::Event::KeyPressed:
					std::cerr << "pressed key\n";
					break;

				default:
					break;
			}
		}

		window.clear();
		//board.draw(window);
		window.display();
	}

	std::cerr << "exiting cleanly\n";
	return 0;
}


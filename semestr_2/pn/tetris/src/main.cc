#include "main.hh"

int main() {
	RotationSystem rotation_system(ROTATION_SYSTEM_PATH);
	ColorScheme color_scheme(COLOR_SCHEME_PATH);

	sf::Window window(sf::VideoMode(800, 600), "TETRIS", sf::Style::Fullscreen);
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
					break;

				case sf::Event::KeyPressed:
					break;

				default:
					break;
			}
		}
		window.clear();
		//matrix.draw(display_center);
		window.display();
	}

	//redraw = false;
	//al_clear_to_color(BLACK);
	//int bx, by, bw, bh;
	//al_get_text_dimensions(kouryuu, "Max's Tetris ripoff", &bx, &by, &bw, &bh);
	//int x = (display_size.w - bw) / 2.0;
	//int y = 0 - by + 20;
	//al_draw_textf(kouryuu, WHITE, x, y, ALLEGRO_ALIGN_INTEGER, "Max's Tetris ripoff");
	//auto display_center = (Vector2) {
	//	display_size.w / 2, display_size.h / 2
	//};

	return 0;
}


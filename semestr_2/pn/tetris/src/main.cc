#include "main.hh"

using namespace std;

Size get_display_size(ALLEGRO_DISPLAY *display) {
	return (Size) { 
		(double) al_get_display_width(display),
		(double) al_get_display_height(display)
	};
}

void draw_rect_fill(Point origin, Size size, ALLEGRO_COLOR color) {
	al_draw_filled_rectangle(origin.x, origin.y, origin.x + size.w, origin.y + size.h, color);
}

void draw_rect_frame(Point origin, Size size, ALLEGRO_COLOR color, double line) {
	al_draw_rectangle(origin.x, origin.y, origin.x + size.w, origin.y + size.h, color, line);
}

int rand_between(int start, int end) {
	return rand() % (end - start) + start;
}

void matrix_init(Matrix *matrix) {
	matrix->rotation_system = read_rotation_system("res/rotation_system/nintendo_left_handed.txt");
	matrix->color_theme = read_color_theme("res/color_theme/plain.txt");
	for (int y = 0; y < MATRIX_H + 2; y += 1) {
		for (int x = 0; x < MATRIX_W + 2; x += 1) {
			Block block;
			block.tetromino_type = 'I';
			block.type = BLOCK_NONE;
			matrix->blocks[y][x] = block;
		}
	}
}

bool matrix_can_place(Matrix *matrix, Tetromino *tetromino, Point point) {
	return false;
}

// Overrides blocks in matrix with given tetromino
void matrix_override(Matrix *matrix, Tetromino *tetromino, Point point, BlockType type) {
	auto system = matrix->rotation_system;
	auto shape = system[tetromino->type][tetromino->rotation];
	auto shape_size = (Size) { 4, 4 };
	Block block;
	block.type = type;
	block.tetromino_type = tetromino->type;
	for (int y = 0; y < shape_size.h; y += 1) {
		for (int x = 0; x < shape_size.w; x += 1) {
			if (shape[y][x] == 1) {
				matrix->blocks[point.y + y][point.x + x] = block;
			}
		}
	}
}

void matrix_spawn(Matrix *matrix, Tetromino *tetromino) {
	matrix_override(matrix, tetromino, {4, 0}, BLOCK_FALLING);
}

void matrix_draw(Matrix *matrix, Point display_center) {
	double block_edge = 16 * 2;
	auto block_size = (Size) {
		block_edge,
		block_edge
	};
	auto frame_size = (Size) {
		(MATRIX_W + 2) * block_size.w,
		(MATRIX_H + 2) * block_size.h
	};
	auto frame_origin = (Point) {
		display_center.x - frame_size.w / 2,
		display_center.y - frame_size.h / 2
	};

	auto buffer_size = (Size) {frame_size.w, block_edge*2};
	draw_rect_frame({frame_origin.x, frame_origin.y + block_edge*2}, 
			buffer_size, WHITE, 2);
	draw_rect_frame(frame_origin, frame_size, WHITE, 1);
	draw_rect_frame(frame_origin, buffer_size, RED, 2);

	for (int y = 0; y < MATRIX_H; y += 1) {
		for (int x = 0; x < MATRIX_W; x += 1) {
			Block block = matrix->blocks[y][x];

			if (block.type != BLOCK_NONE) {
				auto block_color = matrix->color_theme[block.tetromino_type];
				auto block_origin = (Point) {
					frame_origin.x + block_size.w * x,
					frame_origin.y + block_size.h * y
				};
				draw_rect_fill(block_origin, block_size, block_color);
			}
		}
	}
}

int main(int argc, char** argv) {
	Matrix matrix;
	matrix_init(&matrix);

	Tetromino tetromino;
	tetromino.type = 'S';
	tetromino.rotation = 0;
	matrix_spawn(&matrix, &tetromino);

	srand(time(NULL));
	al_init();
	al_init_image_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_mouse();
	al_set_new_display_flags(DISPLAY_OPTIONS);

    auto *display = al_create_display(640, 480);
	auto *timer = al_create_timer(1.0 / FPS);

	auto *kouryuu = al_load_ttf_font(FONT_KOURYUU_PATH,
			FONT_KOURYUU_BASE * 2, FONT_OPTIONS);

	auto *queue = al_create_event_queue();
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_keyboard_event_source());

	al_start_timer(timer);


	auto redraw = false;
	auto last_time = al_get_time();
	auto display_size = get_display_size(display);

	while (true) {

		// wait for events
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		// handle real time
		auto curr_time = al_get_time();
		auto delta_time = curr_time - last_time;
		last_time = curr_time;

		if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		} else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			fprintf(stderr, "[key d] %s\n", al_keycode_to_name(event.keyboard.keycode));
			if (event.keyboard.keycode == ALLEGRO_KEY_R) {
				redraw = true;
			}

		} else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			fprintf(stderr, "[key u] %s\n", al_keycode_to_name(event.keyboard.keycode));

		} else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			display_size = get_display_size(display);
			fprintf(stderr, "[resize] w = %.2f, h = %.2f\n", display_size.w, display_size.h);
			redraw = true;

		} else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		if (redraw && al_is_event_queue_empty(queue)) {
			redraw = false;
			al_clear_to_color(BLACK);
			int bx, by, bw, bh;
			al_get_text_dimensions(kouryuu, "Max's Tetris ripoff", &bx, &by, &bw, &bh);
			int x = (display_size.w - bw) / 2.0;
			int y = 0 - by + 20;
			al_draw_textf(kouryuu, WHITE, x, y, ALLEGRO_ALIGN_INTEGER, "Max's Tetris ripoff");
			auto display_center = (Point) {
				display_size.w / 2, display_size.h / 2
			};
			matrix_draw(&matrix, display_center);
			al_flip_display();
		}

	}

	al_destroy_display(display);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
	fprintf(stderr, "[note] bye!\n");
	return 0;
}


#include "objects.hh"

void Tetromino::rotate() {
	this->rotation += 1;
	this->rotation %= 1;
}

Matrix::Matrix() {
	this->rotation_system = read_rotation_system("res/rotation_system/nintendo_left_handed.txt");
	this->color_theme = read_color_theme("res/color_theme/plain.txt");
	for (int y = 0; y < MATRIX_H + 2; y += 1) {
		for (int x = 0; x < MATRIX_W + 2; x += 1) {
			Block block;
			block.tetromino_type = 'I';
			block.type = BLOCK_NONE;
			this->blocks[y][x] = block;
		}
	}
}

bool Matrix::can_place(Tetromino tetromino, Vector2i point) {
	return false;
}

// Overrides blocks in matrix with given tetromino
void Matrix::place(Tetromino tetromino, Vector2i point) {
	auto system = this->rotation_system;
	auto shape = system[tetromino.type][tetromino.rotation];
	auto shape_size = (Vector2i) { 4, 4 };
	Block block;
	block.type = type;
	block.tetromino_type = tetromino->type;
	for (int y = 0; y < shape_size.x; y += 1) {
		for (int x = 0; x < shape_size.y; x += 1) {
			if (shape[y][x] == 1) {
				this->blocks[point.y + y][point.x + x] = block;
			}
		}
	}
}

void Matrix::spawn(Tetromino tetromino) {
	matrix_override(matrix, tetromino, {4, 0}, BLOCK_FALLING);
}

void Matrix::draw(Vector2i display_center) {
	double block_edge = 16 * 2;
	Vector2i block_size(
		block_edge,
		block_edge
	);
	Vector2i frame_size(
		(MATRIX_W + 2) * block_size.x,
		(MATRIX_H + 2) * block_size.y
	);
	Vector2i frame_origin(
		display_center.x - frame_size.x / 2,
		display_center.y - frame_size.y / 2
	);

	auto buffer_size = (Vector2i) {frame_size.x, block_edge*2};
	draw_rect_frame({frame_origin.x, frame_origin.y + block_edge*2}, 
			buffer_size, WHITE, 2);
	draw_rect_frame(frame_origin, frame_size, WHITE, 1);
	draw_rect_frame(frame_origin, buffer_size, RED, 2);

	for (int y = 0; y < MATRIX_H; y += 1) {
		for (int x = 0; x < MATRIX_W; x += 1) {
			Block block = matrix->blocks[y][x];

			if (block.type != BLOCK_NONE) {
				auto block_color = matrix->color_theme[block.tetromino_type];
				Vector2i block_origin(
					frame_origin.x + block_size.x * x,
					frame_origin.y + block_size.y * y
				);
				draw_rect_fill(block_origin, block_size, block_color);
			}
		}
	}
}

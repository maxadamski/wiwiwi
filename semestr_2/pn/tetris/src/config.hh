#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "utils.hh"
#include <string>
#include <vector>
#include <array>
#include <map>

// The Tetris matrix is 10 blocks wide,
// and at least 22 blocks high.
// Two topmost blocks are hidden.
#define BOARD_W 10
#define BOARD_H 22

typedef std::vector<std::vector<bool>> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;
typedef std::array<int, 3> RGBColor;

enum TetrominoType {
	I = 'I', O = 'O', J = 'J', L = 'L', S = 'S', T = 'T', Z = 'Z'
};

class RotationSystem {
	std::map<TetrominoType, TetrominoRotation> data;

	public:
		RotationSystem(std::string file_path);

		TetrominoShape get_shape(TetrominoType type, int rotation);
};

class ColorScheme {
	std::map<TetrominoType, RGBColor> data;

	public:
		ColorScheme(std::string file_path);

		RGBColor get_color(TetrominoType type);
};

std::vector<char> tetromino_types() {
	return {I, O, J, L, S, T, Z};
}

#endif

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "utils.hh"
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <map>
#include <string>


// The Tetris matrix is 10 blocks wide,
// and at least 22 blocks high.
// Two topmost blocks are hidden.
#define BOARD_W 10
#define BOARD_H 22

typedef char TetrominoType;
typedef std::array<std::array<bool, 4>, 4> TetrominoShape;
typedef std::array<TetrominoShape, 4> TetrominoRotation;
typedef std::map<TetrominoType, TetrominoRotation> RotationSystem;
typedef std::map<TetrominoType, sf::Color> ColorTheme;

const TetrominoType tetromino_names[] = {
	'I', 'O', 'J', 'L', 'S', 'T', 'Z'
};

TetrominoShape tetromino_shape(FILE *file, int size);
RotationSystem rotation_system(std::string path);
ColorTheme color_theme(std::string path);

#endif

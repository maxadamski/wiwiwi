#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "game.hh"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

// The Tetris matrix is 10 blocks wide,
// and at least 22 blocks high.
// Two topmost blocks are hidden.
#define BOARD_W 10
#define BOARD_H 22

#define ROTATION_SYSTEM_PATH "res/rotation_system/nintendo_left_handed.txt"
#define COLOR_SCHEME_PATH "res/color_scheme/pastel.txt"
#define MUSIC_A_PATH "res/audio/tetris_a.ogg"
#define FONT_KOURYUU_PATH "res/font/kouryuu.ttf"
#define FONT_KOURYUU_BASE 11
#define WINDOW_TITLE "Max's Tetris Ripoff"
#define WINDOW_STYLE sf::Style::Fullscreen
#define WINDOW_W 960
#define WINDOW_H 960
#define SONIC_DROP false
#define SHADOW true
#define TETROMINO_POINTS 4

#endif

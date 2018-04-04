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
#define COLOR_SCHEME_PATH "res/color_scheme/plain.txt"
#define FONT_KOURYUU_PATH "res/font/kouryuu.ttf"
#define FONT_KOURYUU_BASE 11
#define WINDOW_TITLE "Max's Tetris Ripoff"
#define WINDOW_STYLE sf::Style::Fullscreen
#define SONIC_DROP false

#endif

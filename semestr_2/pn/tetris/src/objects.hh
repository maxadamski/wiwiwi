#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include "config.hh"
#include "utils.hh"

#include <vector>

#include <SFML/Graphics.hpp>

class Block {
	sf::Color color;

	public:
		Block();
};

class Matrix {
	TetrominoType tetromino_type;

	public:
		std::vector<std::vector<Block>> blocks;
		Vector2 origin, size;

		Matrix();

		void setSize(Vector2 size);
		void setRotation(int rotation);
};

class Board {
	Matrix board;
	Matrix falling_piece;
	Vector2 size;

	public:
		Board();

		void draw(Vector2 origin);
};

#endif

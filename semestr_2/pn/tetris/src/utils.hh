#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cmath>

typedef sf::Vector2i Vector2;

double distance(Vector2 a, Vector2 b) {
	return 0;
}

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

int random(int start, int end) {
	// TODO: use uniform int distribution
	return rand() % (end - start) + start;
}

#endif

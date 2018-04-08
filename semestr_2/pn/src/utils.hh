#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <random>

typedef sf::Vector2i Vec2i;
typedef sf::Vector2f Vec2f;
typedef Vec2i Vec2;
typedef sf::RenderWindow Window;
typedef sf::Color Color;

struct Timer {
	sf::Time duration, remaining;
	bool flag = false;

	Timer(sf::Time duration):
		duration(duration), remaining(sf::seconds(0)) {};

	bool timeout();
	void tick(sf::Time elapsed);
	void reset();
};

std::mt19937 &random_generator();
int random(int min, int max);
void random_seed();

double distance(Vec2 u, Vec2 v);
Vec2f to_f(Vec2i v);
Vec2i to_i(Vec2f v);
Vec2 mul(Vec2 u, int a);
Vec2 mul(Vec2 u, Vec2 v);
Vec2 add(Vec2 u, Vec2 v);

int min(int a, int b);
int max(int a, int b);

#endif

#include "utils.hh"
#include <random>
#include <cmath>

std::mt19937 rng;

int random(int min, int max) {
	std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
	return dist(rng);
}

std::mt19937 &random_generator() {
	return rng;
}

void random_seed() {
	rng.seed(std::random_device()());
}

double distance(Vec2 u, Vec2 v) {
	return sqrt(pow(u.x - v.x, 2) + pow(u.y - v.y, 2));
}

Vec2f to_f(Vec2i v) {
	return Vec2f(v.x, v.y);
}

Vec2i to_i(Vec2f v) {
	return Vec2i((int) v.x, (int) v.y);
}

Vec2 mul(Vec2 u, int a) {
	return Vec2(a * u.x, a * u.y);
}

Vec2 mul(Vec2 u, Vec2 v) {
	return Vec2(u.x * v.x, u.y * v.y);
}

Vec2 add(Vec2 u, Vec2 v) {
	return Vec2(u.x + v.x, u.y + v.y);
}

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}


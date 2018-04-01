#include "config.hh"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

// TODO: handle errors in user input

RGBColor color_from_hex(std::string s) {
	if (s[0] == '0' && s[1] == 'x')
		s.erase(0, 2);
	if (s[0] == '#')
		s.erase(0, 1);
	int hex;
	sscanf(s.c_str(), "%x", &hex);
	return {hex & 0xFF0000, hex & 0x00FF00, hex & 0x0000FF};
}

RotationSystem::RotationSystem(std::string file_path) {
	using namespace std;
	ifstream data(file_path);

	for (auto type : tetromino_types()) {
		int size = (type == I || type == O) ? 4 : 3;
		TetrominoRotation rotation;

		for (int n = 0; n < 4; n++) {
			TetrominoShape shape;
			shape.resize(size);

			for (int y = 0; y < size; y++) {
				shape[y].resize(size);
				string line; getline(data, line);
				stringstream stream(line);

				for (int x = 0; x < size; x++) {
					string byte; getline(stream, byte, ' ');
					shape[y][x] = stoi(byte);
				}
			}

			rotation[n] = shape;
		}

		auto ttype = static_cast<TetrominoType>(type);
		this->data[ttype] = rotation;
	}
}

TetrominoShape RotationSystem::get_shape(TetrominoType type, int rotation) {
	return this->data[type][rotation % 4];
}

ColorScheme::ColorScheme(std::string file_path) {
	using namespace std;
	ifstream data(file_path);
	for (size_t i = 0; i < tetromino_types().size(); i++) {
		string line; getline(data, line);
		stringstream stream(line);
		string type, hex;
		getline(stream, type, ' ');
		getline(stream, hex, ' ');
		auto ttype = static_cast<TetrominoType>(type[0]);
		this->data[ttype] = color_from_hex(hex);
	}
}

RGBColor ColorScheme::get_color(TetrominoType type) {
	return this->data[type];
}

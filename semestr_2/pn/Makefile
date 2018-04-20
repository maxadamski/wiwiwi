run: build
	build/tetris

build: clean
	mkdir -p build
	g++ -g -Wall --std=c++17 -o build/tetris src/*.cc -pthread \
		`pkg-config --cflags --libs sfml-graphics sfml-window sfml-system sfml-audio`

clean:
	rm -rf build

run: build
	build/tetris

build: clean
	mkdir -p build
	g++ -g -o build/tetris src/main.cc -pthread \
		`pkg-config --cflags --libs allegro-5 allegro_image-5 allegro_font-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 allegro_color-5`

clean:
	rm -rf build

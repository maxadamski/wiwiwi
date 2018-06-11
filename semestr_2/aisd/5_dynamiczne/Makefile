.PHONY: clean build bench test plot

clean:
	rm -rf build input output

build:
	rm -rf build; mkdir -p build
	mkdir -p output
	g++ -O0 --std=c++17 \
		-o build/benchmark -I ../testkit \
		src/benchmark.cc ../testkit/testkit.cc

quality: build
	build/benchmark --quality > output/quality.csv

speed: build
	build/benchmark --speed > output/speed.csv

spread: build
	build/benchmark --spread > output/3d.csv

bench: quality speed

test: build
	build/benchmark --test

plot:
	python3 src/plot.py

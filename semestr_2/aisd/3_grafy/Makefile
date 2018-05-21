.PHONY: clean input build bench test plot

clean:
	rm -rf build output input

input:
	mkdir -p input
	unzip -nj input.zip -d input

build:
	rm -rf build; mkdir -p build
	mkdir -p output
	g++ -O0 --std=c++17 \
		-o build/benchmark -I ../testkit \
		src/benchmark.cc ../testkit/testkit.cc

bench: build
	build/benchmark --tsort > output/tsort.csv
	build/benchmark --edges > output/edges.csv

test: build
	build/benchmark --test

plot:
	python3 src/plot.py

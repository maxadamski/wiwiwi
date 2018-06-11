.PHONY: clean input build bench test plot

clean:
	rm -rf build input output

input:
	mkdir -p input
	python3 src/generate.py > input/names.csv

build:
	rm -rf build; mkdir -p build
	mkdir -p output
	g++ -g -O0 --std=c++14 \
		-o build/benchmark -I ../testkit \
		src/benchmark.cc ../testkit/testkit.cc

bench: build
	build/benchmark --bench > output/benchmark.csv

test: build
	build/benchmark --test

plot:
	python3 src/massage.py
	python3 src/plot.py

.PHONY: clean build bench test plot

clean:
	rm -rf build output

build:
	rm -rf build; mkdir -p build
	mkdir -p output
	g++ -O0 --std=c++14 \
		-o build/benchmark -I ../testkit \
		src/benchmark.cc ../testkit/testkit.cc

bench: build
	build/benchmark --bench > output/bench.csv

test: build
	build/benchmark --test

plot:
	python3 src/plot.py

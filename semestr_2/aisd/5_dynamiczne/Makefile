.PHONY: all build run plot

all: build run plot

build:
	@rm -rf build; mkdir -p build
	g++ -g -O0 --std=c++17 -o build/benchmark src/benchmark.cc src/testkit.cc

run:
	@mkdir -p output
	build/benchmark > output/bench.csv

plot:
	python3 script/plot.py

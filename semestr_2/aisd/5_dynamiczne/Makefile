.PHONY: all clean build results run report

all: clean data run results report

clean:
	rm -rf build
	rm -rf output

setup:
	mkdir -p build
	mkdir -p output

build: clean setup
	g++ -g -O0 --std=c++17 -o build/benchmark src/benchmark.cc src/testkit.cc

run: build
	build/benchmark > output/benchmark.csv

results:
	python3 script/plot.py

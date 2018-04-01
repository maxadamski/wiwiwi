.PHONY: plot run build

plot:
	rm -rf plots
	mkdir -p plots

run: build
	build/benchmark > 1_sortowanie.csv

build:
	rm -rf build
	mkdir -p build
	g++ -g -O0 --std=c++17 -o build/benchmark benchmark.cc testkit.cc

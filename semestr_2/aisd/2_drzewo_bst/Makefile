.PHONY: plot run build

all: data run plot

clean:
	rm -rf build
	rm -rf data

data:
	mkdir -p data
	python3 script/make_data.py > data/names.csv

plot:
	mkdir -p data/plots
	python3 script/make_plots.py

run: build
	build/benchmark > data/bench.csv

build:
	mkdir -p build
	g++ -g -O0 --std=c++17 -o build/benchmark benchmark.cc testkit.cc

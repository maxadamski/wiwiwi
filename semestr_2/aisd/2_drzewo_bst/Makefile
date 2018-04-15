.PHONY: all clean build results run

all: clean data run results

clean:
	rm -rf build
	rm -rf data

build:
	mkdir -p build
	g++ -g -O0 --std=c++17 -o build/benchmark src/benchmark.cc src/testkit.cc

data:
	mkdir -p data
	python3 script/generate.py > data/names.csv
	cp -f reference.csv data/

run: build
	build/benchmark > data/benchmark.csv

results:
	python3 script/massage.py
	python3 script/plot.py


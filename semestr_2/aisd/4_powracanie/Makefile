.PHONY: all clean build results run report

all: clean data run results report

clean:
	rm -rf build
	rm -rf output

build:
	mkdir -p build
	g++ -g -O0 --std=c++17 -o build/benchmark src/benchmark.cc src/testkit.cc

output:
	mkdir -p output
	#python3 script/generate.py > data/names.csv
	#cp -f reference.csv data/

run: build
	build/benchmark --euler > output/euler.csv
	build/benchmark --hamilton > output/hamilton.csv

results:
	#python3 script/massage.py
	python3 script/plot.py

.PHONY: plot run build

plot:
	rm -rf plots
	mkdir -p plots
	python3 1_sortowanie.py

run: build
	build/1_sortowanie > 1_sortowanie.csv

build:
	rm -rf build
	mkdir -p build
	g++ -O0 --std=c++14 -o build/1_sortowanie 1_sortowanie.cc testkit.cc

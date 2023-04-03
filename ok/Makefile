.PHONY: clean build test time

clean:
	rm -rf out

build:
	@mkdir -p out
	g++ --std=c++17 -o out/jobshop src/*.cc

test:
	script/test -t -m greedy

time:
	script/test -b out/res.csv -m greedy

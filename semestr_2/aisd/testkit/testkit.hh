#ifndef TESTKIT_H
#define TESTKIT_H

// standard
#include <cstdlib>
#include <cstring>

// io
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// containers
#include <string>
#include <vector>
#include <queue>
#include <list>

// magic
#include <algorithm>
#include <functional>
#include <random>
#include <chrono>

struct Timer {
	// time in nanoseconds
	int duration, remaining;
	bool flag = false;

	Timer(int duration):
		duration(duration),
		remaining(duration) {};

	bool timeout();
	void tick(int elapsed);
	void reset();
};

// Zwraca losowa licze z zakresu ['min','max']
// Przed uzyciem zawolaj 'random_seed'
int random(int min, int max);
void random_seed();

void shuffle(std::vector<int> &vector);

// Zwraca uśredniony czas wykonywania f-i 'lambda' w nanosekundach
// 'passes': ilosc powtorzen
// 'print_passes': dla kazdego powtorzenia wypisz czas ktory uplynal
long int benchmark(int passes,
	std::function<void()> measure,
	std::function<void()> before,
	std::function<void()> after,
	bool print_passes = false);

long int benchmark(int passes,
	std::function<void()> measure);

long int benchmark_ms(int passes,
	std::function<void()> measure);

long int benchmark_simple(std::function<void()> measure);

// Kopiuje tablice array i zwraca wskaznik.
// Pamietaj o free!
int *copy_array(int* array, int length);

void print(int* array, int length);

void print(std::vector<int> v);

void print(std::list<int> v);

std::vector<std::string> parse_args(int argc, char **argv);

#endif

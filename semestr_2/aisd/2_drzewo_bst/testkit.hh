#ifndef TESTKIT_H
#define TESTKIT_H

#include <functional>
#include <vector>

// Zwraca losowa licze z zakresu ['min','max']
// Przed uzyciem zawolaj 'random_seed'
int random(int min, int max);
void random_seed();

void shuffle(std::vector<int> vector);

// Zwraca uśredniony czas wykonywania f-i 'lambda' w nanosekundach
// 'passes': ilosc powtorzen
// 'print_passes': dla kazdego powtorzenia wypisz czas ktory uplynal
long int benchmark(int passes, bool print_passes,
	std::function<void()> before,
	std::function<void()> measure,
	std::function<void()> after);

long int benchmark_simple(std::function<void()> measure);

// Kopiuje tablice array i zwraca wskaznik.
// Pamietaj o free!
int *copy_array(int* array, int length);

void print_array(int* array, int length);

#endif

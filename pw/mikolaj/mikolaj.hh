// Autor: Dariusz Max Adamski

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include <random>
#include <string>

//
// Configuration
//

#define DEBUG
// how many reindeers
#define DEER_COUNT 9
// how many elves
#define HELP_COUNT 10
// total employee count (do not change)
#define AGENT_COUNT (DEER_COUNT + HELP_COUNT)
// minimum reindeers to wake up santa
#define DEER_MIN 9
// minimum elves to wake up santa
#define HELP_MIN 3
// how many seconds in an hour
#define HOUR 2
// probability of a reindeer waking up
#define DEER_WAKE_PROB 15
// probability of an elf waking up
#define HELP_WAKE_PROB 15
// duration of present-giving event
#define PRESENT_TIME 10
// duration of executive-meeting event
#define MEETING_TIME 10
// how long must santa sleep after an event
#define SANTA_SLEEP 0
// wait time between drawing status
#define DRAW_WAIT 5e4

//
// Data Structures
//

enum AgentStatus {
	IDLE = ' ', WAIT = '.', BUSY = '#'
};

enum SantaStatus {
	SLEEP = 0, PRESENTS, MEETING
};

std::string to_string(SantaStatus status) {
	switch (status) {
		case SLEEP: return "sleeping";
		case MEETING: return "holding an executive meeting";
		case PRESENTS: return "giving presents to nice students";
		default: return "???";
	}
}

//
// Boilerplate
//

#define CLEAR "\e[1;1H\e[2J"

#define MUTEX_INIT PTHREAD_MUTEX_INITIALIZER
#define COND_INIT PTHREAD_COND_INITIALIZER

#define P pthread_mutex_lock
#define V pthread_mutex_unlock
#define wait pthread_cond_wait
#define signal pthread_cond_signal
#define broadcast pthread_cond_broadcast

std::mt19937 generator;
std::uniform_int_distribution<std::mt19937::result_type> dist(1, 100);

void random_seed() {
	generator.seed(std::random_device()());
}

bool random_chance(unsigned long probability) {
	return dist(generator) <= probability;
}


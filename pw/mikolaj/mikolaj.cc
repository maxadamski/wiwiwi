// Autor: Dariusz Max Adamski

#include "mikolaj.hh"
#include <iostream>

using std::cerr;

volatile AgentStatus status[AGENT_COUNT];
volatile SantaStatus santa_status;

pthread_mutex_t santa_status_mutex = MUTEX_INIT;
pthread_mutex_t agent_status_mutex = MUTEX_INIT;

pthread_cond_t santa_sleep = COND_INIT, santa_done = COND_INIT;
pthread_cond_t agent_busy[2] = { COND_INIT, COND_INIT };
pthread_cond_t agent_wait[2] = { COND_INIT, COND_INIT };

// agent-type-dependent values
SantaStatus event_status[] = { PRESENTS, MEETING };
int event_duration[] = { PRESENT_TIME, MEETING_TIME };
int WAKE_PROB[] = { DEER_WAKE_PROB, HELP_WAKE_PROB };
int AGENT_EVENT[] = { PRESENTS, MEETING };

struct AgentArgs {
	// agent type: 0 -> reindeer, 1 -> elf
	int id, type;
};

// count the number of reindeer/elves with a given status
int status_count(bool helpers, AgentStatus key) {
	int start = helpers ? DEER_COUNT : 0;
	int steps = helpers ? HELP_COUNT : DEER_COUNT;
	int count = 0;
	for (int i = 0; i < steps; i++)
		if (status[start + i] == key) count++;
	return count;
}

void *agent(void *thread_args) {
	AgentArgs args = *((AgentArgs*) thread_args);
	int id = args.id, type = args.type;
	// get agent parameters depending on the agent type
	int wake_prob = WAKE_PROB[type];
	int agent_event = AGENT_EVENT[type];
	status[id] = IDLE;

	while (1) {
		P(&agent_status_mutex);
		status[id] = IDLE;
		V(&agent_status_mutex);

		// sleep for an hour, and then
		// randomly wake up or sleep for another hour
		do sleep(HOUR); while (!random_chance(wake_prob));

		// set status to wait and try to wake up santa
		P(&agent_status_mutex);
		status[id] = WAIT;
		signal(&santa_sleep);
		V(&agent_status_mutex);

		// wait until santa starts the event
		P(&santa_status_mutex);
		while (santa_status != agent_event)
			wait(&agent_wait[type], &santa_status_mutex);
		V(&santa_status_mutex);

		// get to work
		P(&agent_status_mutex);
		status[id] = BUSY;
		V(&agent_status_mutex);

		// agent participates in the event,
		// while santa hosts the event
		P(&santa_status_mutex);
		while (santa_status == agent_event)
			wait(&agent_busy[type], &santa_status_mutex);
		V(&santa_status_mutex);

		// agent leaves the event and goes to sleep
		P(&agent_status_mutex);
		status[id] = IDLE;
		// tell santa that you left
		signal(&santa_done);
		V(&agent_status_mutex);
	}
}

void *santa(void *thread_args) {
	(void) thread_args; // silence unused parameter warning
	while (1) {
		// santa must sleep for a little while
		P(&santa_status_mutex);
		santa_status = SLEEP;
		V(&santa_status_mutex);

		sleep(SANTA_SLEEP);

		// santa sleeps until enough agents arrive
		P(&agent_status_mutex);
		while (status_count(0, WAIT) < DEER_MIN && status_count(1, WAIT) < HELP_MIN)
			wait(&santa_sleep, &agent_status_mutex);
		// santa's behaviour changes depending on the agent_type
		// reindeer have priority, of course
		int agent_type = status_count(0, WAIT) >= DEER_MIN ? 0 : 1;
		V(&agent_status_mutex);

		// santa starts the event
		P(&santa_status_mutex);
		santa_status = event_status[agent_type];
		// tell the waiting agents to get to work
		broadcast(&agent_wait[agent_type]);
		V(&santa_status_mutex);

		sleep(event_duration[agent_type]);

		// santa ends the event
		P(&santa_status_mutex);
		santa_status = SLEEP;
		// tell the busy agents to leave
		broadcast(&agent_busy[agent_type]);
		V(&santa_status_mutex);

		// santa waits for the agents to leave
		P(&agent_status_mutex);
		// until none of the agents are busy
		while (status_count(agent_type, BUSY) > 0)
			wait(&santa_done, &agent_status_mutex);
		V(&agent_status_mutex);
	}
}

void *draw(void *thread_args) {
	(void) thread_args; // silence unused parameter warning
	while (1) {
		write(1, CLEAR, 12);
		cerr << " reindeer: (" << DEER_COUNT - status_count(0, IDLE) << ") [";
		for (int i = 0; i < DEER_COUNT; i++) cerr << (char) status[i];
		cerr << "] helpers: (" << HELP_COUNT - status_count(1, IDLE) << ") [";
		for (int i = DEER_COUNT; i < AGENT_COUNT; i++) cerr << (char) status[i];
		cerr << "] santa: [" << to_string(santa_status) << "]\n";
		usleep(DRAW_WAIT);
	}
}

int main() {
	random_seed();
	pthread_t santa_thread, draw_thread;
	pthread_t agent_thread[AGENT_COUNT];
	AgentArgs agent_arg[AGENT_COUNT];
	for (int i = 0; i < AGENT_COUNT; i++) {
		agent_arg[i] = { i, i >= DEER_COUNT };
		pthread_create(&agent_thread[i], NULL, &agent, (void*) &agent_arg[i]);
	}
	pthread_create(&santa_thread, NULL, &santa, NULL);
	pthread_create(&draw_thread, NULL, &draw, NULL);
	pthread_join(santa_thread, NULL);
	return 0;
}


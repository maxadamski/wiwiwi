#include "main.hh"

// SECTION: Data Types

enum Tag { REQ, OK, END };
struct Msg { Tag tag; i32 time; i32 queue; };

// SECTION: Configuration

// maximum number of tickets for one event
i32 const max_tickets = 6;
// number of workshops
i32 const num_workshops = 3;
// maximum number of workshops one can attend
i32 const max_workshops = 3;
// number of slots in each workshop
i32 const max_places = 2;
// workshop duration
f64 const max_time = 2;
// sleep between yields
i32 const sleep_time = 2000;
// shuffle workshops
bool const should_shuffle = true;

// SECTION: Global State

// true when recv awaits for a message
bool recv_await = false;
// the received message
Msg msg;
// maximum global rank (const after init)
i32 max_rank;
// local rank (const after init)
i32 my_rank;
// local lamport clock
i32 my_time = 0;

bool executing[num_workshops+1] = {false};
bool requesting[num_workshops+1] = {false};
vector<i32> deferred[num_workshops+1];
i32 accepted[num_workshops+1] = {0};
i32 exited = 0;

#define debug(format, ...) fprintf(stderr, "%4d P%d " format, my_time, my_rank, ##__VA_ARGS__)

void tick(i32 new_time = 0) {
	my_time = max(my_time, new_time) + 1;
}

MPI_Request request = {0};
MPI_Status status = {0};

bool recv(i32 *src, Msg *msg) {

	if (!recv_await) {
		MPI_Irecv(msg, sizeof(Msg), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
		recv_await = true;
	}

	i32 completed;
	MPI_Test(&request, &completed, &status);
	if (!completed) return false;

	*src = status.MPI_SOURCE;
	recv_await = false;
	return true;
}

void send(i32 dst, Tag tag, i32 queue) {
	Msg msg {.tag = tag, .time = my_time, .queue = queue};
	MPI_Send(&msg, sizeof(Msg), MPI_BYTE, dst, 0, MPI_COMM_WORLD);
}

void respond(i32 req_time = -1) {
	i32 src;
	if (!recv(&src, &msg)) return;
	tick(msg.time);

	if (msg.tag == REQ) {
		bool lower_priority = msg.time == req_time ? src < my_rank : msg.time < req_time;
		if ((!requesting[msg.queue] && !executing[msg.queue]) || (requesting[msg.queue] && lower_priority)) {
			tick();
			send(src, OK, msg.queue);
		} else {
			append(deferred[msg.queue], src);
		}
	} else if (msg.tag == OK) {
		accepted[msg.queue] += 1;
	} else if (msg.tag == END) {
		exited += 1;
	}
}

void P(i32 queue_id, i32 capacity) {
	tick();
	i32 req_time = my_time;
	for (i32 rank = 0; rank < max_rank; rank++)
		if (rank != my_rank)
			send(rank, REQ, queue_id);

	requesting[queue_id] = true;
	accepted[queue_id] = 0;
	while (accepted[queue_id] < max_rank - capacity) {
		respond(req_time);
		usleep(sleep_time);
	}

	//debug("executing %d (%d >= %d)\n", queue_id, accepted[queue_id], max_rank - capacity);
	requesting[queue_id] = false;
	executing[queue_id] = true;
}

void V(i32 queue_id) {
	tick();
	for (i32 rank : deferred[queue_id])
		send(rank, OK, queue_id);
	deferred[queue_id].clear();
	executing[queue_id] = false;
}

void JOIN() {
	exited += 1;

	tick();
	for (i32 rank : range(0, max_rank))
		if (rank != my_rank)
			send(rank, END, -1);

	while (exited < max_rank)
		respond();
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &max_rank);

	debug("my rank %d (max rank %d)\n", my_rank, max_rank);

	for (i32 iter = 1; true; iter++) {
		vector<i32> workshops = range(1, num_workshops + 1);
		if (should_shuffle) shuffle(workshops);
		exited = 0;

		debug("plan %s\n", to_string(workshops).c_str());
		debug(YLW "will enter event" NLC);
		P(0, max_tickets);
		debug(GRN "did  enter event" NLC);
		for (i32 i = 0; i < max_workshops; i++) {
			i32 w = workshops[i];
			debug(YLW "will enter W%d" NLC, w);
			P(w, max_places);
			debug(GRN "did  enter W%d" NLC, w);
			i32 start = clock();
			while (f64(clock() - start) / CLOCKS_PER_SEC < max_time) {
				respond();
			}
			V(w);
			debug(RED "did  exit  W%d" NLC, w);
		}
		V(0);
		debug(RED "did  exit  event" NLC);

		JOIN();
		debug(RED "-- iter %d ----------------" NLC, iter);
	}

	MPI_Finalize();
	return 0;
}

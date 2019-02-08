// Autor: Dariusz Max Adamski

#include "palacze.hh"

volatile int *shared, *price, *wallet;

#define WALLET_INIT 15
#define PRICE_MIN 1
#define PRICE_MAX 10
#define SMOKE_BREAK 1e6
#define SMOKERS 3
#define MUTEXES 3
#define LOCKS (3 * SMOKERS)
#define SEMAPHORES (MUTEXES + LOCKS)

enum Semaphore {
	agent_mutex   = 0,
	market_mutex  = 1,
	wallet_mutex  = 2,
	wallet_lock   = MUTEXES + 0*SMOKERS,
	smoking_lock  = MUTEXES + 1*SMOKERS,
	consumer_lock = MUTEXES + 2*SMOKERS
};

void agent() {
	while (1) {
		P(market_mutex);

		for (bool any = false; !any;) {
			for (int i = 0; i < SMOKERS; i++) {
				price[i] = random_int(PRICE_MIN, PRICE_MAX);
			}

			printf("---| 0: %2d$ | 1: %2d$ | 2: %2d$ |---\n", price[0], price[1], price[2]);

			for (int i = 0; i < SMOKERS; i++) {
				int cost = 0;
				for (int j = 0; j < SMOKERS; j++)
					if (j != i) cost += price[j];
				if (wallet[i] >= cost) {
					V(consumer_lock + i);
					printf("%d [%2d$] wake need %2d$\n", i, wallet[i], cost);
					any = true;
					break;
				}
			}
		}

		V(market_mutex);
		P(agent_mutex);
	}
}

void smoker(int id) {
	Message msg;

	if (fork()) {
		while (1) {
			//P(smoking_lock + id);
			RECV(&msg, id);

			int cash = msg.mvalue[0], user = msg.mvalue[1];

			P(wallet_lock + id);
			wallet[id] += cash;
			printf("%d [%2d$] recv %2d$ <-- %d\n", id, wallet[id], cash, user);
			V(wallet_lock + id);
			//V(smoking_lock + id);
		}
	} else {
		while (1) {
			P(consumer_lock + id);
			P(market_mutex);
			
			for (int i = 1; i < SMOKERS; i++) {
				int user = (id + i) % SMOKERS;
				int cost = price[user];

				msg.mvalue[1] = id;
				msg.mvalue[0] = cost;

				printf("%d [%2d$] send %2d$ --> %d\n", id, wallet[id], cost, user);
				SEND(&msg, user);
				P(wallet_lock + id);
				wallet[id] -= cost;
				V(wallet_lock + id);
			}

			V(agent_mutex);
			V(market_mutex);

			//P(smoking_lock + id);
			printf("%d [%2d$] smoke\n", id, wallet[id]);
			usleep(SMOKE_BREAK);
			printf("%d [%2d$] sleep\n", id, wallet[id]);
			//V(smoking_lock + id);
		}
	}
}

int main(void) {
	setbuf(stdout, NULL);
	srand(time(NULL));
	msgd = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	semd = semget(IPC_PRIVATE, SEMAPHORES, IPC_CREAT | 0600);
	shmd = shmget(IPC_PRIVATE, 2*SMOKERS * sizeof(int), IPC_CREAT | 0600);

	// initialize semaphores
	for (int i = 0; i < SEMAPHORES; i++)
		semctl(semd, i, SETVAL, 1);
	for (int i = 0; i < SMOKERS; i++)
		semctl(semd, i + consumer_lock, SETVAL, 0);
	semctl(semd, agent_mutex, SETVAL, 0);

	// get access to shared memory
	shared = (volatile int*) shmat(shmd, NULL, 0);
	wallet = shared;
	price = shared + SMOKERS;

	// initialize shared memory
	for (int i = 0; i < SMOKERS; i++)
		wallet[i] = WALLET_INIT;

	// spawn smokers
	for (int i = 0; i < SMOKERS; i++) {
		if (!fork()) {
			smoker(i);
			exit(0);
		}
	}

	agent();
	return 0;
}


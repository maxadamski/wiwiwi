// Autor: Dariusz Max Adamski

#include "palacze.hh"

volatile int *shared, *price, *wallet;

#define MUTEXES 2
#define LOCKS (2 * SMOKERS)
#define SEMAPHORES (MUTEXES + LOCKS)

enum Semaphore {
	agent_mutex   = 0,
	market_mutex  = 1,
	wallet_lock   = MUTEXES + 0*SMOKERS,
	consumer_lock = MUTEXES + 1*SMOKERS,
};

void agent() {
	while (1) {
		// the ingredient prices cannot be read by anyone else, while setting them
		P(market_mutex);

		// while no smoker can afford the ingredients
		bool any = false;
		while (!any) {
			// randomly set ingredient prices 
			for (int i = 0; i < SMOKERS; i++)
				price[i] = random_int(PRICE_MIN, PRICE_MAX);

#ifdef BUFFER_FIX
			printf("---| 0: %2d$ | 1: %2d$ | 2: %2d$ |---\n",
					price[0], price[1], price[2]);
#else
			
			printf("---|");
			for (int i = 0; i < SMOKERS; i++)
				printf(" %d: %2d$ |", i, price[i]);
			printf("---\n");
#endif

			// compute the cost of all ingredients
			int cost = 0;
			for (int i = 0; i < SMOKERS; i++)
				cost += price[i];

			for (int i = 0; !any && i < SMOKERS; i++) {
				// safely check the i-th wallet
				// but the wallet can only increase here anyway...
				P(wallet_lock + i);
				int wallet_i = wallet[i];
				V(wallet_lock + i);

				// if the smoker can afford the ingredients
				// not counting their own ingredient, of course
				int need = cost - price[i];
				if (wallet_i >= need) {
					// wake up the i-th smoker
					V(consumer_lock + i);
					printf("%d [%2d$] wake need %2d$\n", i, wallet[i], need);
					any = true;
				}
			}
		}

		V(market_mutex);
		// wait for the transaction to finish
		P(agent_mutex);
	}
}

void smoker(int id) {
	Message msg;

	if (fork()) {
		while (1) {
			// wait for cash
			RECV(&msg, id);
			int cash = msg.mvalue[0];
			int user = msg.mvalue[1];

			// safely add received cash to the wallet
			P(wallet_lock + id);
			wallet[id] += cash;
			printf("%d [%2d$] recv %2d$ <-- %d\n", id, wallet[id], cash, user);
			V(wallet_lock + id);
		}
	} else {
		while (1) {
			// wait until we can afford the ingredients
			P(consumer_lock + id);
			// the ingredient prices cannot change during the transaction
			P(market_mutex);
			
			// for each ingredient, except ours
			for (int i = 1; i < SMOKERS; i++) {
				// get the id of the smoker who sells the ingredient, and it's price
				int user = (id + i) % SMOKERS;
				int cost = price[user];
				msg.mvalue[0] = cost;
				msg.mvalue[1] = id;

				// buy it and send the money
				printf("%d [%2d$] send %2d$ --> %d\n", id, wallet[id], cost, user);
				SEND(&msg, user);

				// safely subtract the amount from the wallet
				P(wallet_lock + id);
				wallet[id] -= cost;
				V(wallet_lock + id);
			}

			// wake up the agent, so someone else can buy
			V(agent_mutex);
			V(market_mutex);

			// have a nice smoke break
			printf("%d [%2d$] smoke\n", id, wallet[id]);
			usleep(SMOKE_BREAK);
			printf("%d [%2d$] sleep\n", id, wallet[id]);
		}
	}
}

int main(void) {
#ifdef BUFFER_FIX
	setbuf(stdout, NULL);
#endif
	srand(time(NULL));
	msgd = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	semd = semget(IPC_PRIVATE, SEMAPHORES, IPC_CREAT | 0600);
	shmd = shmget(IPC_PRIVATE, 2*SMOKERS*sizeof(int), IPC_CREAT | 0600);

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

	// the parent becomes the agent
	agent();
	return 0;
}


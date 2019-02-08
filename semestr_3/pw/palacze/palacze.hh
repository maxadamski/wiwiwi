#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>

#define msize (sizeof(Message) - sizeof(long))

struct Message {
	long mtype;
	int mvalue[2];
};

struct sembuf sem_buf;

int semd, shmd, msgd;

void P(int semnum) {
	sem_buf.sem_num = semnum;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = 0;
	semop(semd, &sem_buf, 1);
}

void V(int semnum) {
	if (semctl(semd, semnum, GETVAL) > 0) return;
	sem_buf.sem_num = semnum;
	sem_buf.sem_op = 1;
	sem_buf.sem_flg = 0;
	semop(semd, &sem_buf, 1);
}

int RECV(Message *message, int mask) {
	// assert(mask > 0)
	return msgrcv(msgd, message, msize, mask + 1, 0);
}

int SEND(Message *message, int mask) {
	message->mtype = mask + 1;
	return msgsnd(msgd, message, msize, 0);
}

int random_int(int min, int max) {
	return rand() % (max - min + 1) + min;
}

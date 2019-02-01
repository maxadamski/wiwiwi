// Autor: Dariusz Max Adamski

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char **argv) {
	printf("hello, world\n");
	return 0;
}


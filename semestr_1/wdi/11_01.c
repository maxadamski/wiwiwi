#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

void NewThread(pthread_t *h, void *p(void *arg)) {
  int Err;
  Err = pthread_create(h, NULL, p, NULL);
  if (Err)
    fprintf(stderr, "ERROR in NewThread: %d\n", Err);
}

void WaiThread(pthread_t h) {
  int Err;
  Err = pthread_join(h, NULL);
  if (Err)
    fprintf(stderr, "ERROR in WaiThread: %d\n", Err);
}

int S = 2;
sem_t mutex;

void *P1(void *unused) {
  int x;
  fprintf(stderr, "P1: wait\n");
  sem_wait(&mutex);
  x = S;
  fprintf(stderr,"P1: x = %d\n", x);
  x += 2;
  S = x;
  fprintf(stderr,"P1: S = %d\n", S);
  fprintf(stderr, "P1: post\n");
  sem_post(&mutex);
  return NULL;
}

void *P2(void *unused) {
  int y;
  fprintf(stderr, "P2: wait 1\n");
  sem_wait(&mutex);
  fprintf(stderr, "P2: wait 2\n");
  sem_wait(&mutex);
  y = S;
  fprintf(stderr,"P2: y = %d\n", y);
  y += 4;
  S = y;
  fprintf(stderr, "P2: S = %d\n", S);
  fprintf(stderr, "P2: post\n");
  sem_post(&mutex);
  return NULL;
}

int main() {
  pthread_t p1, p2;
  sem_init(&mutex, 0, 1);
  fprintf(stderr, "newthread P1\n");
  NewThread(&p1, P1);
  fprintf(stderr, "newthread P2\n");
  NewThread(&p2, P2);
  fprintf(stderr, "sleep 20\n");
  sleep(20);
  fprintf(stderr, "%d\n", S);
  fprintf(stderr, "waithread P1\n");
  WaiThread(p1);
  fprintf(stderr, "waithread P2\n");
  WaiThread(p2);
  return 0;
}

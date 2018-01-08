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

int S = 20;
sem_t mutex;

void *P1(void *unused) {
  int x;
  sem_post(&mutex);
  x = S;
  fprintf(stderr,"P1: x = %d\n", x);
  x += 10;
  S = x;
  sem_post(&mutex);
  fprintf(stderr, "P1: S = %d\n", S);
  return NULL;
}

void *P2(void *unused) {
  int y;
  sem_post(&mutex);
  y = S;
  fprintf(stderr, "P2: y = %d\n", y);
  y += 20;
  S = y;
  sem_post(&mutex);
  fprintf(stderr, "P2: S = %d\n", S);
  return NULL;
}

int main() {
  pthread_t p1, p2;
  sem_init(&mutex, 0, 1);
  NewThread(&p1, P1);
  NewThread(&p2, P2);
  WaiThread(p1);
  WaiThread(p2);
  fprintf(stderr, "%d\n", S);
  return 0;
}

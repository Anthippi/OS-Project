#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5

void *PrintHello(void *threadid) {
  long tid = *((long *)(threadid));
  printf("Hello from thread #%ld!\n", tid);
  pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
  pthread_t threads[NUM_THREADS]; // πίνακας από νήματα
  int rc; long t;
  for(t=0; t < NUM_THREADS; t++){
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, PrintHello, (void *)&t);
    if (rc){
      printf("ERROR code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  pthread_exit(NULL); // ΠΑΝΤΑ!!! Αλλιώς τερματίζουν και τα νήματα
}

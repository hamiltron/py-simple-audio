#include "simpleaudio.h"
#include <stdlib.h>
#include <pthread.h>

void* create_mutex() {
  void* mutex;
  mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
  return mutex;
}

void destroy_mutex(void* mutex) {
  pthread_mutex_destroy((pthread_mutex_t*)mutex);
  free(mutex);
}

void grab_mutex(void* mutex) {
  pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void release_mutex(void* mutex) {
  pthread_mutex_unlock((pthread_mutex_t*)mutex);
}
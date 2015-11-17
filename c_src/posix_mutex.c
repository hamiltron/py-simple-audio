/*
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton
MIT License (see LICENSE.txt)
*/

#include "simpleaudio.h"
#include <stdlib.h>
#include <pthread.h>

void* create_mutex() {
  void* mutex;
  mutex = PyMem_Malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
  return mutex;
}

void destroy_mutex(void* mutex) {
  pthread_mutex_destroy((pthread_mutex_t*)mutex);
  PyMem_Free(mutex);
}

void grab_mutex(void* mutex) {
  pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void release_mutex(void* mutex) {
  pthread_mutex_unlock((pthread_mutex_t*)mutex);
}
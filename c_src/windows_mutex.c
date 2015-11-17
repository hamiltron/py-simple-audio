/*
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton
MIT License (see LICENSE.txt)
*/

#include "simpleaudio.h"
#include <stdlib.h>
#include <Windows.h>

void* create_mutex() {
  void* mutex;
  mutex = (void*)CreateMutex(NULL, FALSE, NULL);
  return mutex;
}

void destroy_mutex(void* mutex) {
  CloseHandle((HANDLE)mutex);
}

void grab_mutex(void* mutex) {
  WaitForSingleObject((HANDLE)mutex, INFINITE);
}

void release_mutex(void* mutex) {
  ReleaseMutex((HANDLE)mutex);
}
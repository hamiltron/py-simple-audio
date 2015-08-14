#ifndef SIMPLEAUDIO_H
#define SIMPLEAUDIO_H

#include <Python.h>
#include <stdio.h>

#define SA_ERR_STR_LEN (256)
#define SIMPLEAUDIO_BUFSZ (4096)

#define SA_CLEAR (0)
#define SA_STOP (1)

#define DBG_OUT stdout
#define DBG_PRE "[dbg] "

enum {
  NOT_LAST_ITEM = 0,
  LAST_ITEM = 1
};

typedef unsigned long long play_id_t;
typedef unsigned long long len_samples_t;

/* linked list structure used to track the active playback items/threads */
typedef struct playItem_s {
  /* the play_id of the list head is used to store the next play_id value
     used by a new play list item */
  play_id_t playId;
  int stopFlag;
  struct playItem_s* prevItem;
  struct playItem_s* nextItem;
  /* the mutex of the list head is used as a 'global' mutex for modifying
     and accessing the list itself */
  void* mutex;
} playItem_t;

/* prototypes */
PyObject* play_os(void* audioData, len_samples_t lenSamples, int numChannels, int bytesPerChan, int sampleRate, playItem_t* playListHead);
    
void deleteListItem(playItem_t* playItem);
playItem_t* newListItem(playItem_t* listHead);
play_id_t new_play_id(playItem_t list_head);

void* create_mutex();
void destroy_mutex(void* mutex);
void grab_mutex(void* mutex);
void release_mutex(void* mutex);

#endif /* SIMPLEAUDIO_H */
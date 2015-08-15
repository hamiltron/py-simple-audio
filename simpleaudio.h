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
typedef struct play_item_s {
    /* the play_id of the list head is used to store the next play_id value
       used by a new play list item */
    play_id_t play_id;
    int stop_flag;
    struct play_item_s* prev_item;
    struct play_item_s* next_item;
    /* the mutex of the list head is used as a 'global' mutex for modifying
       and accessing the list itself */
    void* mutex;
} play_item_t;

/* prototypes */
PyObject* play_os(void* audio_data, len_samples_t len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head);

void delete_list_item(play_item_t* play_item);
play_item_t* new_list_item(play_item_t* list_head);

void* create_mutex(void);
void destroy_mutex(void* mutex);
void grab_mutex(void* mutex);
void release_mutex(void* mutex);

#endif /* SIMPLEAUDIO_H */
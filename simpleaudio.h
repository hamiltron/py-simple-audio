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

/* some handy macros for debug prints used in ultiple places */
#if DEBUG > 0
    #define DBG_PLAY_OS_CALL \
        fprintf(DBG_OUT, DBG_PRE"play_os call: buffer at %p, %llu samples, %d channels, %d bytes-per-chan, sample rate %d, list head at %p\n", \
                buffer_obj.buf, len_samples, num_channels, bytes_per_chan, sample_rate, play_list_head);

    #define DBG_DESTROY_BLOB fprintf(DBG_OUT, DBG_PRE"destroying audio blob at %p\n", audio_blob);

    #define DBG_CREATE_BLOB fprintf(DBG_OUT, DBG_PRE"created audio blob at %p\n", audio_blob);
#else
    #define DBG_PLAY_OS_CALL
    #define DBG_DESTROY_BLOB
    #define DBG_CREATE_BLOB
#endif

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

extern PyObject* sa_python_error;

/* prototypes */
PyObject* play_os(Py_buffer buffer_obj, len_samples_t len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head);

void delete_list_item(play_item_t* play_item);
play_item_t* new_list_item(play_item_t* list_head);

void* create_mutex(void);
void destroy_mutex(void* mutex);
void grab_mutex(void* mutex);
void release_mutex(void* mutex);

#endif /* SIMPLEAUDIO_H */
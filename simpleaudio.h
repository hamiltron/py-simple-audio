/* 
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton 
MIT License (see LICENSE.txt)
*/

#ifndef SIMPLEAUDIO_H
#define SIMPLEAUDIO_H

#include <Python.h>
#include <stdio.h>

#define SA_ERR_STR_LEN (256)

#define SA_CLEAR (0)
#define SA_STOP (1)

#define SA_LATENCY_US (100000)

#define DBG_OUT stdout
#define DBG_PRE "[dbg] "

/* debug print function prototypes */
void dbg1(const char* str, ...);
void dbg2(const char* str, ...);

/* some handy macros for debug prints used in multiple places */
#define DBG_PLAY_OS_CALL \
    dbg1("play_os call: ...\n");\
    dbg1(" ... audio data at %p\n", buffer_obj.buf);\
    dbg1(" ... list head at %p\n", play_list_head);\
    dbg1(" ... %d samples\n", len_samples);\
    dbg1(" ... %d channels\n", num_channels);\
    dbg1(" ... %d bytes per channel\n", bytes_per_chan);\
    dbg1(" ... %d Hz sample rate\n", sample_rate);\
    dbg1(" ... %d microseconds latency\n", latency_us);

#define DBG_DESTROY_BLOB dbg1("destroying audio blob at %p\n", audio_blob);

#define DBG_CREATE_BLOB dbg1("created audio blob at %p\n", audio_blob);

enum {
    NOT_LAST_ITEM = 0,
    LAST_ITEM = 1
};

typedef unsigned long long play_id_t;

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

typedef struct {
    Py_buffer buffer_obj;
    void* handle;
    int used_bytes;
    int len_bytes;
    int num_buffers;
    int frame_size;
    int buffer_size;
    play_item_t* play_list_item;
    void* list_mutex;
} audio_blob_t;

extern PyObject* sa_python_error;

/* prototypes */
PyObject* play_os(Py_buffer buffer_obj, int len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head, int latency_us);

void delete_list_item(play_item_t* play_item);
play_item_t* new_list_item(play_item_t* list_head);

void destroy_audio_blob(audio_blob_t* audio_blob);
audio_blob_t* create_audio_blob(void);

int get_buffer_size(int latency_us, int sample_rate, int frame_size);

void* create_mutex(void);
void destroy_mutex(void* mutex);
void grab_mutex(void* mutex);
void release_mutex(void* mutex);

#endif /* SIMPLEAUDIO_H */
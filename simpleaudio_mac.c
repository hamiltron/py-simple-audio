/* 
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton 
MIT License (see LICENSE.txt)
*/

#include "simpleaudio.h"
#include <AudioToolbox/AudioToolbox.h>

/* this should be replaced by whatever constant Apple has designated */
#define SUCCESS (0)

#define NUM_BUFS (2)

#define MAC_EXCEPTION(my_msg, code, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d", my_msg, code); \
    PyErr_SetString(sa_python_error, str_ptr);

typedef struct {
    Py_buffer buffer_obj;
    int used_bytes;
    int len_bytes;
    int num_buffers;
    play_item_t* play_list_item;
    void* list_mutex;
} mac_audio_blob_t;

void destroy_audio_blob(mac_audio_blob_t* audio_blob) {
    PyGILState_STATE gstate;
    
    DBG_DESTROY_BLOB

    /* release the buffer view so Python can
       decrement it's refernce count*/
    gstate = PyGILState_Ensure();
    PyBuffer_Release(&audio_blob->buffer_obj);
    PyGILState_Release(gstate);

    grab_mutex(audio_blob->list_mutex);
    delete_list_item(audio_blob->play_list_item);
    release_mutex(audio_blob->list_mutex);
    PyMem_Free(audio_blob);
}

/* NOTE: like the official example code,
   OSX API calls are not checked for errors here */
static void audio_callback(void* param, AudioQueueRef audio_queue, AudioQueueBuffer *queue_buffer) {
    mac_audio_blob_t* audio_blob = (mac_audio_blob_t*)param;
    int want = queue_buffer->mAudioDataBytesCapacity;
    int have = audio_blob->len_bytes-audio_blob->used_bytes;
    int stop_flag;

    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"audio_callback call with audio blob at %p\n", param);
    #endif

    grab_mutex(audio_blob->play_list_item->mutex);
    stop_flag = audio_blob->play_list_item->stop_flag;
    release_mutex(audio_blob->play_list_item->mutex);

    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"stop flag: %d\n", stop_flag);
    #endif

    /* if there's still audio yet to buffer ... */
    if (have > 0 && !stop_flag) {
        #if DEBUG > 1
        fprintf(DBG_OUT, DBG_PRE"still feeding queue\n");
        #endif

        if (have > want) {have = want;}
        memcpy(queue_buffer->mAudioData, audio_blob->buffer_obj.buf + (size_t)(audio_blob->used_bytes), have);
        queue_buffer->mAudioDataByteSize = have;
        audio_blob->used_bytes += have;
        AudioQueueEnqueueBuffer(audio_queue, queue_buffer, 0, NULL);
    /* ... no more audio left to buffer */
    } else {
        #if DEBUG > 1
        fprintf(DBG_OUT, DBG_PRE"done enqueue'ing - dellocating a buffer\n");
        #endif

        if (audio_blob->num_buffers > 0) {
            AudioQueueFreeBuffer(audio_queue, queue_buffer);
            audio_blob->num_buffers--;
        }
        if (audio_blob->num_buffers == 0) {
            /* all done, cleanup */
            AudioQueueStop(audio_queue, true);
            AudioQueueDispose(audio_queue, true);
            destroy_audio_blob(audio_blob);
        }
    }
}

PyObject* play_os(Py_buffer buffer_obj, int len_samples, int num_channels, int bytes_per_chan, 
                  int sample_rate, play_item_t* play_list_head, int latency_us) {
    char err_msg_buf[SA_ERR_STR_LEN];
    AudioQueueRef audio_queue;
    AudioStreamBasicDescription audio_fmt;
    AudioQueueBuffer* queue_buffer;
    /* signed 32-bit int - I think */
    OSStatus result;
    mac_audio_blob_t* audio_blob;
    size_t bytesPerFrame = bytes_per_chan * num_channels;
    int buffer_size;
    int i;

    DBG_PLAY_OS_CALL

    buffer_size = get_buffer_size(latency_us / NUM_BUFS, sample_rate, bytes_per_chan);

    /* audio blob creation and audio buffer copy */
    audio_blob = PyMem_Malloc(sizeof(mac_audio_blob_t));

    DBG_CREATE_BLOB

    audio_blob->buffer_obj = buffer_obj;
    audio_blob->list_mutex = play_list_head->mutex;
    audio_blob->len_bytes = len_samples * bytesPerFrame;
    audio_blob->used_bytes = 0;
    audio_blob->num_buffers = 0;

    /* setup the linked list item for this playback buffer */
    grab_mutex(play_list_head->mutex);
    audio_blob->play_list_item = new_list_item(play_list_head);
    release_mutex(play_list_head->mutex);

    /* mac format header setup */
    memset(&audio_fmt, 0, sizeof(audio_fmt));
    audio_fmt.mSampleRate = sample_rate;
    audio_fmt.mFormatID = kAudioFormatLinearPCM;
    audio_fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audio_fmt.mFramesPerPacket = 1;
    audio_fmt.mChannelsPerFrame = num_channels;
    audio_fmt.mBytesPerFrame = bytesPerFrame;
    audio_fmt.mBytesPerPacket = audio_fmt.mBytesPerFrame * audio_fmt.mFramesPerPacket;
    audio_fmt.mBitsPerChannel = bytes_per_chan * 8;

    result = AudioQueueNewOutput(&audio_fmt, audio_callback, audio_blob, NULL, NULL, 0, &audio_queue);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unsupported audio format?", result, err_msg_buf);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    result = AudioQueueSetParameter(audio_queue, kAudioQueueParam_Volume, 1.0);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unable to set volume.", result, err_msg_buf);
        AudioQueueDispose(audio_queue,true);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"allocating %d queue buffers of %d bytes\n", NUM_BUFS, buffer_size);
    #endif
    for(i = 0; i < NUM_BUFS; i++) {
        result = AudioQueueAllocateBuffer(audio_queue, buffer_size, &queue_buffer);
        if (result != SUCCESS) {
            MAC_EXCEPTION("Unable to allocate buffer.", result, err_msg_buf);
            AudioQueueDispose(audio_queue,true);
            destroy_audio_blob(audio_blob);
            return NULL;
        }
        audio_blob->num_buffers++;
        /* fill a buffer using the callback */
        audio_callback(audio_blob, audio_queue, queue_buffer);
    }

    result = AudioQueueStart(audio_queue, NULL);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unable to start queue.", result, err_msg_buf);
        AudioQueueDispose(audio_queue,true);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    return PyLong_FromUnsignedLongLong(audio_blob->play_list_item->play_id);
}


#include "simpleaudio.h"
#include <AudioToolbox/AudioToolbox.h>

/* this should be replaced by whatever constant Apple has designated */
#define SUCCESS (0)

#define NUM_Q_BUFS (2)

#define MAC_EXCEPTION(my_msg, code, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d", my_msg, code); \
    PyErr_SetString(PyExc_Exception, str_ptr);

typedef struct {
    void* audio_buffer;
    int used_bytes;
    int len_bytes;
    int buffers;
    play_item_t* play_list_item;
    void* list_mutex;
} mac_audio_blob_t;

void destroy_audio_blob(mac_audio_blob_t* audio_blob) {
    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"destroying audio blob at %p\n", audio_blob);
    #endif

    PyMem_Free(audio_blob->audio_buffer);
    grab_mutex(audio_blob->list_mutex);
    delete_list_item(audio_blob->play_list_item);
    release_mutex(audio_blob->list_mutex);
    PyMem_Free(audio_blob);
}

/* NOTE: like the official example code,
   OSX API calls are not checked for errors here */
static void audio_callback(void* param, AudioQueueRef audio_queue, AudioQueueBuffer *queue_buffer) {
    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"audio_callback call with audio blob at %p\n", param);
    #endif

    mac_audio_blob_t* audio_blob = (mac_audio_blob_t*)param;
    int want = queue_buffer->mAudioDataBytesCapacity;
    int have = audio_blob->len_bytes-audio_blob->used_bytes;
    int stop_flag;

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
        memcpy(queue_buffer->mAudioData, &audio_blob->audio_buffer[audio_blob->used_bytes], have);
        queue_buffer->mAudioDataByteSize = have;
        audio_blob->used_bytes += have;
        AudioQueueEnqueueBuffer(audio_queue, queue_buffer, 0, NULL);
    /* ... no more audio left to buffer */
    } else {
        #if DEBUG > 1
        fprintf(DBG_OUT, DBG_PRE"done enqueue'ing - dellocating a buffer\n");
        #endif

        if (audio_blob->buffers > 0) {
            AudioQueueFreeBuffer(audio_queue, queue_buffer);
            audio_blob->buffers--;
        }
        if (audio_blob->buffers == 0) {
            /* all done, cleanup */
            AudioQueueStop(audio_queue, true);
            AudioQueueDispose(audio_queue, true);
            destroy_audio_blob(audio_blob);
        }
    }
}

PyObject* play_os(void* audio_data, len_samples_t len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head) {
    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"play_os call: buffer at %p, %llu samples, %d channels, %d bytes-per-chan, sample rate %d, list head at %p\n", audio_data, len_samples, num_channels, bytes_per_chan, sample_rate, play_list_head);
    #endif

    char err_msg_buf[SA_ERR_STR_LEN];
    AudioQueueRef audio_queue;
    AudioStreamBasicDescription audio_fmt;
    AudioQueueBuffer* queue_buffer;
    /* signed 32-bit int - I think */
    OSStatus result;
    mac_audio_blob_t* audio_blob;
    size_t bytesPerFrame = bytes_per_chan * num_channels;
    int i;

    /* audio blob creation and audio buffer copy */
    audio_blob = PyMem_Malloc(sizeof(mac_audio_blob_t));

    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"created audio blob at %p\n", audio_blob);
    #endif

    audio_blob->list_mutex = play_list_head->mutex;
    audio_blob->audio_buffer = PyMem_Malloc(len_samples * bytesPerFrame);
    audio_blob->len_bytes = len_samples * bytesPerFrame;
    memcpy(audio_blob->audio_buffer, audio_data, len_samples * bytesPerFrame);
    audio_blob->used_bytes = 0;
    audio_blob->buffers = 0;
    memset(&audio_fmt, 0, sizeof(audio_fmt));

    /* setup the linked list item for this playback buffer */
    grab_mutex(play_list_head->mutex);
    audio_blob->play_list_item = new_list_item(play_list_head);
    release_mutex(play_list_head->mutex);

    /* mac format header setup */
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

    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"allocating %d queue buffers of %d bytes\n", NUM_Q_BUFS, SIMPLEAUDIO_BUFSZ);
    #endif
    for(i = 0; i < NUM_Q_BUFS; i++) {
        result = AudioQueueAllocateBuffer(audio_queue, SIMPLEAUDIO_BUFSZ, &queue_buffer);
        if (result != SUCCESS) {
            MAC_EXCEPTION("Unable to allocate buffer.", result, err_msg_buf);
            AudioQueueDispose(audio_queue,true);
            destroy_audio_blob(audio_blob);
            return NULL;
        }
        audio_blob->buffers++;
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


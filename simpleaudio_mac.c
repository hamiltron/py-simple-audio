#include "simpleaudio.h"
#include <AudioToolbox/AudioToolbox.h>

/* this should be replaced by whatever constant Apple has designated */
#define SUCCESS (0)

#define NUM_Q_BUFS (2)

#define MAC_EXCEPTION(my_msg, code, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d", my_msg, code); \
    PyErr_SetString(PyExc_Exception, str_ptr);
    
typedef struct {
    void* audioBuffer;
    int usedBytes;
    int lenBytes; 
    int buffers;
    playItem_t* playListItem;
    void* list_mutex;
} macAudioBlob_t;

macAudioBlob_t* createAudioBlob(void) {
    macAudioBlob_t* audioBlob = PyMem_Malloc(sizeof(macAudioBlob_t));
    
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"created audio blob at %p\n", audioBlob);
    #endif
    
    return audioBlob;
}

void destroyAudioBlob(macAudioBlob_t* audioBlob) {
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"destroying audio blob at %p\n", audioBlob);
    #endif
    
    PyMem_Free(audioBlob->audioBuffer);
    grab_mutex(audioBlob->list_mutex);
    deleteListItem(audioBlob->playListItem);
    release_mutex(audioBlob->list_mutex);
    PyMem_Free(audioBlob);
}

/* NOTE: like the official example code, 
   OSX API calls are not checked for errors here */
static void audioCallback(void* param, AudioQueueRef audioQueue, AudioQueueBuffer *queueBuffer) {
    #if DEBUG == 2
    fprintf(DBG_OUT, DBG_PRE"audio_callback call with audio blob at %p\n", param);
    #endif
    
    macAudioBlob_t* audioBlob = (macAudioBlob_t*)param;
    int want = queueBuffer->mAudioDataBytesCapacity;
    int have = audioBlob->lenBytes-audioBlob->usedBytes; 
    int stopFlag;
  
    grab_mutex(audioBlob->playListItem->mutex);
    stopFlag = audioBlob->playListItem->stopFlag;
    release_mutex(audioBlob->playListItem->mutex);
    
    #if DEBUG == 2
    fprintf(DBG_OUT, DBG_PRE"stop flag: %d\n", stopFlag);
    #endif
    
    /* if there's still audio yet to buffer ... */
    if (have > 0 && !stopFlag) {
        #if DEBUG == 2
        fprintf(DBG_OUT, DBG_PRE"still feeding queue\n");
        #endif
        
        if (have > want) {have = want;}
        memcpy(queueBuffer->mAudioData, &audioBlob->audioBuffer[audioBlob->usedBytes], have);
        queueBuffer->mAudioDataByteSize = have;
        audioBlob->usedBytes += have;
        AudioQueueEnqueueBuffer(audioQueue, queueBuffer, 0, NULL);
    /* ... no more audio left to buffer */
    } else {
        #if DEBUG == 2
        fprintf(DBG_OUT, DBG_PRE"done enqueue'ing - dellocating a buffer\n");
        #endif

        if (audioBlob->buffers > 0) { 
            AudioQueueFreeBuffer(audioQueue, queueBuffer); 
            audioBlob->buffers--; 
        }
        if (audioBlob->buffers == 0) {
            /* all done, cleanup */
            AudioQueueStop(audioQueue, true);
            AudioQueueDispose(audioQueue, true);
            destroyAudioBlob(audioBlob);
        }
    }
}

PyObject* play_os(void* audioData, len_samples_t lenSamples, int numChannels, int bytesPerChan, int sampleRate, playItem_t* playListHead) {
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"play_os call: buffer at %p, %llu samples, %d channels, %d bytes-per-chan, sample rate %d, list head at %p\n", audioData, lenSamples, numChannels, bytesPerChan, sampleRate, playListHead);
    #endif
    
    char err_msg_buf[SA_ERR_STR_LEN];
    AudioQueueRef audioQueue;
    AudioStreamBasicDescription audioFmt;
    AudioQueueBuffer* queueBuffer;
    /* signed 32-bit int - I think */
    OSStatus result;
    macAudioBlob_t* audioBlob;
    size_t bytesPerFrame = bytesPerChan * numChannels;
    int i;
    
    /* initial allocation and audio buffer copy */
    audioBlob = createAudioBlob();
    audioBlob->list_mutex = playListHead->mutex;
    audioBlob->audioBuffer = PyMem_Malloc(lenSamples * bytesPerFrame);
    audioBlob->lenBytes = lenSamples * bytesPerFrame;
    memcpy(audioBlob->audioBuffer, audioData, lenSamples * bytesPerFrame);
    audioBlob->usedBytes = 0;
    audioBlob->buffers = 0;
    memset(&audioFmt, 0, sizeof(audioFmt));
    
    /* setup the linked list item for this playback buffer */
    grab_mutex(playListHead->mutex);
    audioBlob->playListItem = newListItem(playListHead);
    release_mutex(playListHead->mutex);
    
    /* mac format header setup */
    audioFmt.mSampleRate = sampleRate;
    audioFmt.mFormatID = kAudioFormatLinearPCM;
    audioFmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFmt.mFramesPerPacket = 1;
    audioFmt.mChannelsPerFrame = numChannels;
    audioFmt.mBytesPerFrame = bytesPerFrame;
    audioFmt.mBytesPerPacket = audioFmt.mBytesPerFrame * audioFmt.mFramesPerPacket;
    audioFmt.mBitsPerChannel = bytesPerChan * 8;
    
    result = AudioQueueNewOutput(&audioFmt, audioCallback, audioBlob, NULL, NULL, 0, &audioQueue);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unsupported audio format?", result, err_msg_buf);
        destroyAudioBlob(audioBlob);
        return NULL;
    }
    
    result = AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, 1.0);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unable to set volume.", result, err_msg_buf);
        AudioQueueDispose(audioQueue,true);
        destroyAudioBlob(audioBlob);
        return NULL;
    }
    
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"allocating %d queue buffers of %d bytes\n", NUM_Q_BUFS, SIMPLEAUDIO_BUFSZ);
    #endif
    for(i = 0; i < NUM_Q_BUFS; i++) {
        result = AudioQueueAllocateBuffer(audioQueue, SIMPLEAUDIO_BUFSZ, &queueBuffer);
        if (result != SUCCESS) {
            MAC_EXCEPTION("Unable to allocate buffer.", result, err_msg_buf);
            AudioQueueDispose(audioQueue,true);
            destroyAudioBlob(audioBlob);
            return NULL;
        }
        audioBlob->buffers++;
        /* fill a buffer using the callback */
        audioCallback(audioBlob, audioQueue, queueBuffer); 
    }
    
    result = AudioQueueStart(audioQueue, NULL);
    if(result != SUCCESS) {
        MAC_EXCEPTION("Unable to start queue.", result, err_msg_buf);
        AudioQueueDispose(audioQueue,true);
        destroyAudioBlob(audioBlob);
        return NULL;
    }
  
    return PyLong_FromUnsignedLongLong(audioBlob->playListItem->playId);
}


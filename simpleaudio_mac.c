#include "simpleaudio.h"
#include <AudioToolbox/AudioToolbox.h>
#include <stdlib.h>

/* this should be replaced by whatever constant Apple has designated */
#define SUCCESS (0)

typedef struct {
  void* audioBuffer;
  int usedBytes;
  int lenBytes; 
  int buffers;
  playItem_t* playListItem;
} macAudioBlob_t;

macAudioBlob_t* createAudioBlob(void) {
  macAudioBlob_t* audioBlob = malloc(sizeof(macAudioBlob_t));
  return audioBlob;
}

void destroyAudioBlob(macAudioBlob_t* audioBlob) {
  void* mutex = audioBlob->playListItem->mutex;
  int lastItemStatus;
  
  free(audioBlob->audioBuffer);
  grab_mutex(mutex);
  lastItemStatus = deleteListItem(audioBlob->playListItem);
  release_mutex(mutex);
  if (lastItemStatus == LAST_ITEM) {
    destroy_mutex(mutex);
  }
  free(audioBlob);
}

static void audioCallback(void* param, AudioQueueRef audioQueue, AudioQueueBuffer *queueBuffer)
{
  macAudioBlob_t* audioBlob = (macAudioBlob_t*)param;
  int want = queueBuffer->mAudioDataBytesCapacity;
  int have = audioBlob->lenBytes-audioBlob->usedBytes; 
  int stopFlag;

  grab_mutex(audioBlob->playListItem->mutex);
  stopFlag = audioBlob->playListItem->stopFlag;
  release_mutex(audioBlob->playListItem->mutex);
  
  /* if there's still audio yet to buffer ... */
  if (have > 0 && !stopFlag) {
    if (have > want) {have = want;}
    memcpy(queueBuffer->mAudioData, &audioBlob->audioBuffer[audioBlob->usedBytes], have);
    queueBuffer->mAudioDataByteSize = have;
    audioBlob->usedBytes += have;
    AudioQueueEnqueueBuffer(audioQueue, queueBuffer, 0, NULL);
  /* ... no more audio left to buffer */
  } else {
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

simpleaudioError_t playOS(void* audioData, int lenSamples, int numChannels, 
    int bitsPerChan, int sampleRate, playItem_t* playListHead) {
  simpleaudioError_t error = {SIMPLEAUDIO_OK, 0, "", ""};
  AudioQueueRef audioQueue;
  AudioStreamBasicDescription audioFmt;
  AudioQueueBuffer* queueBuffer;
  OSStatus result;
  macAudioBlob_t* audioBlob;
  size_t bytesPerChan = bitsPerChan / 8;
  size_t bytesPerFrame = bytesPerChan * numChannels;
  int i;
  
  /* initial allocation and audio buffer copy */
  audioBlob = createAudioBlob();
  audioBlob->audioBuffer = malloc(lenSamples * bytesPerFrame);
  audioBlob->lenBytes = lenSamples * bytesPerFrame;
  memcpy(audioBlob->audioBuffer, audioData, lenSamples * bytesPerFrame);
  audioBlob->usedBytes = 0;
  audioBlob->buffers = 0;
  memset(&audioFmt, 0, sizeof(audioFmt));
  
  /* setup the linked list item for this playback buffer */
  grab_mutex(playListHead->mutex);
  audioBlob->playListItem = newListItem(playListHead);
  audioBlob->playListItem->playId = 0;
  audioBlob->playListItem->stopFlag = 0;
  release_mutex(playListHead->mutex);
  
  /* mac format header setup */
  audioFmt.mSampleRate = sampleRate;
  audioFmt.mFormatID = kAudioFormatLinearPCM;
  audioFmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
  audioFmt.mFramesPerPacket = 1;
  audioFmt.mChannelsPerFrame = numChannels;
  audioFmt.mBytesPerFrame = bytesPerFrame;
  audioFmt.mBytesPerPacket = audioFmt.mBytesPerFrame * audioFmt.mFramesPerPacket;
  audioFmt.mBitsPerChannel = bitsPerChan;
  
  result = AudioQueueNewOutput(&audioFmt, audioCallback, audioBlob, NULL, NULL, 0, &audioQueue);
  if(result != SUCCESS) {
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.apiMessage, "Unsupported audio format?", SIMPLEAUDIO_ERRMSGLEN);
    destroyAudioBlob(audioBlob);
    return error;
  }
  
  result = AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, 1.0);
  if(result != SUCCESS) {
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.apiMessage, "Unable to set volume.", SIMPLEAUDIO_ERRMSGLEN);
    AudioQueueDispose(audioQueue,true);
    destroyAudioBlob(audioBlob);
    return error;
  }
  
  for(i = 0; i < 2; i++) {
    result = AudioQueueAllocateBuffer(audioQueue, SIMPLEAUDIO_BUFSZ, &queueBuffer);
    if (result != SUCCESS) {
      error.errorState = SIMPLEAUDIO_ERROR;
      error.code = (simpleaudioCode_t)result;
      strncpy(error.apiMessage, "Unable to allocate buffer.", SIMPLEAUDIO_ERRMSGLEN);
      AudioQueueDispose(audioQueue,true);
      destroyAudioBlob(audioBlob);
      return error;
    }
    audioBlob->buffers++;
    /* fill a buffer using the callback */
    audioCallback(audioBlob, audioQueue, queueBuffer); 
  }
  
  result = AudioQueueStart(audioQueue, NULL);
  if(result != SUCCESS) {
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.apiMessage, "Unable to start queue.", SIMPLEAUDIO_ERRMSGLEN);
    AudioQueueDispose(audioQueue,true);
    destroyAudioBlob(audioBlob);
    return error;
  }

  return error;
}


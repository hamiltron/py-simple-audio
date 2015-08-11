#include "simpleaudio.h"
#include <asoundlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define RESAMPLE (1)
#define LATENCY_US (100000) /* 100 ms */

typedef struct { 
  void* audioBuffer;
  snd_pcm_t* handle;
  int samplesLeft; 
  int samplesPlayed;
  int frameSize;
  playItem_t* playListItem;
} alsaAudioBlob_t;

alsaAudioBlob_t* createAudioBlob(void) {
  alsaAudioBlob_t* audioBlob = malloc(sizeof(alsaAudioBlob_t));
  audioBlob->audioBuffer = NULL;
  audioBlob->handle = NULL; 
  return audioBlob;
}

void destroyAudioBlob(alsaAudioBlob_t* audioBlob) {
  void* mutex = audioBlob->playListItem->mutex;
  int lastItemStatus;
  
  free(audioBlob->audioBuffer);
  grabMutex(mutex);
  lastItemStatus = deleteListItem(audioBlob->playListItem);
  releaseMutex(mutex);
  if (lastItemStatus == LAST_ITEM) {
    destroyMutex(mutex);
  }
  free(audioBlob);
}

void* playbackThread(void* threadParam) {
  alsaAudioBlob_t* audioBlob = (alsaAudioBlob_t*)threadParam;
  void* audioPtr;
  int playSamples;
  int result;
  int stopFlag = 0;

  while (audioBlob->samplesLeft > 0 && !stopFlag) {
    grabMutex(audioBlob->playListItem->mutex);
    stopFlag = audioBlob->playListItem->stopFlag;
    releaseMutex(audioBlob->playListItem->mutex);
   
    if (audioBlob->samplesLeft < SIMPLEAUDIO_BUFSZ / audioBlob->frameSize) {
      playSamples = audioBlob->samplesLeft;
    } else {
      playSamples = SIMPLEAUDIO_BUFSZ / audioBlob->frameSize;
    }
    audioPtr = audioBlob->audioBuffer + 
        (size_t)(audioBlob->samplesPlayed * audioBlob->frameSize);
    result = snd_pcm_writei(audioBlob->handle, audioPtr, playSamples);
    if (result < 0) {
      result = snd_pcm_recover(audioBlob->handle, result, 0);
      if (result < 0) {break;} /* unrecoverable error */
    } else {
      audioBlob->samplesPlayed += result;
      audioBlob->samplesLeft -= result;
    }
  }
  
  snd_pcm_drain(audioBlob->handle);
  snd_pcm_close(audioBlob->handle);
  destroyAudioBlob(audioBlob);
  
  pthread_exit(0);
}

simpleaudioError_t playOS(void* audioData, int lenSamples, int numChannels, 
    int bitsPerChan, int sampleRate, playItem_t* playListHead) {
  alsaAudioBlob_t* audioBlob;
  simpleaudioError_t error = {SIMPLEAUDIO_OK, 0, "", ""};
  int bytesPerChan = bitsPerChan / 8;
  int bytesPerFrame = bytesPerChan * numChannels; 
  static char *device = "default";
  snd_pcm_format_t sampleFormat;
  pthread_t playThread;
  int result;
  
  /* set that format appropriately */
  if (bytesPerChan == 2) {
    sampleFormat = SND_PCM_FORMAT_S16_LE;
  } else {
    error.errorState = SIMPLEAUDIO_ERROR;
    strncpy(error.apiMessage, "Unsupported Sample Format.", SIMPLEAUDIO_ERRMSGLEN);
    return error;
  }
  
  /* initial allocation and audio buffer copy */
  audioBlob = createAudioBlob();
  audioBlob->audioBuffer = malloc(lenSamples * bytesPerFrame);
  memcpy(audioBlob->audioBuffer, audioData, lenSamples * bytesPerFrame);
  audioBlob->samplesLeft = lenSamples;
  audioBlob->samplesPlayed = 0;
  audioBlob->frameSize = bytesPerFrame;
  
  /* setup the linked list item for this playback buffer */
  grabMutex(playListHead->mutex);
  audioBlob->playListItem = newListItem(playListHead);
  audioBlob->playListItem->playId = 0;
  audioBlob->playListItem->stopFlag = 0;
  releaseMutex(playListHead->mutex);
  
  /* open access to a PCM device (blocking mode)  */
  result = snd_pcm_open(&audioBlob->handle, device, SND_PCM_STREAM_PLAYBACK, 0);
  if (result < 0) {	
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.sysMessage, snd_strerror(result), SIMPLEAUDIO_ERRMSGLEN);
    strncpy(error.apiMessage, "Error opening PCM device.", SIMPLEAUDIO_ERRMSGLEN);

    destroyAudioBlob(audioBlob);
    return error;
	}
  /* set the PCM params */
  result = snd_pcm_set_params(audioBlob->handle, sampleFormat, SND_PCM_ACCESS_RW_INTERLEAVED,
    numChannels, sampleRate, RESAMPLE, LATENCY_US);
  if (result < 0) {	
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.sysMessage, snd_strerror(result), SIMPLEAUDIO_ERRMSGLEN);
    strncpy(error.apiMessage, "Error setting parameters.", SIMPLEAUDIO_ERRMSGLEN);
    
    snd_pcm_close(audioBlob->handle);
    destroyAudioBlob(audioBlob);
    return error;
	}

  /* fire off the playback thread (and hope the ALSA calls don't fail) */
  result = pthread_create(&playThread, NULL, playbackThread, (void*)audioBlob);
  if (result != 0) {
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    strncpy(error.apiMessage, "Could not create playback thread.", SIMPLEAUDIO_ERRMSGLEN);
    
    snd_pcm_close(audioBlob->handle);
    destroyAudioBlob(audioBlob);
    return error;
  }
  
  return error;
}


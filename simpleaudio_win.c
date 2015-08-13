#include "simpleaudio.h"
#include <Windows.h>
#include <Mmsystem.h>
#include <stdlib.h>

enum {
  FILL_BUFFER_WRITE = 0,
  FILL_BUFFER_DRAINING = 1,
  FILL_BUFFER_DONE = 2
};

typedef struct {
  void* audioBuffer;
  HWAVEOUT waveOutHdr;
  int usedBytes;
  int lenBytes; 
  int numBuffers;
  playItem_t* playListItem;
} winAudioBlob_t;

winAudioBlob_t* createAudioBlob(void) {
  winAudioBlob_t* audioBlob = PyMem_Malloc(sizeof(winAudioBlob_t));
  audioBlob->audioBuffer = NULL;
  return audioBlob;
}

void destroyAudioBlob(winAudioBlob_t* audioBlob) {
  void* mutex = audioBlob->playListItem->mutex;
  int lastItemStatus;
  
  PyMem_Free(audioBlob->audioBuffer);
  grabMutex(mutex);
  lastItemStatus = deleteListItem(audioBlob->playListItem);
  releaseMutex(mutex);
  if (lastItemStatus == LAST_ITEM) {
    destroyMutex(mutex);
  }
  PyMem_Free(audioBlob);
}

MMRESULT fillBuffer(WAVEHDR* waveHeader, winAudioBlob_t* audioBlob) {
  int want = waveHeader->dwBufferLength;
  int have = audioBlob->lenBytes - audioBlob->usedBytes; 
  int stopFlag = 0;
  MMRESULT result;
  
  grabMutex(audioBlob->playListItem->mutex);
  stopFlag = audioBlob->playListItem->stopFlag;
  releaseMutex(audioBlob->playListItem->mutex);
  
  /* if there's still audio yet to buffer ... */
  if (have > 0 && !stopFlag) {
    if (have > want) {have = want;}
    result = waveOutUnprepareHeader(audioBlob->waveOutHdr, waveHeader, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    memcpy(waveHeader->lpData, &((char*)audioBlob->audioBuffer)[audioBlob->usedBytes], have);
    result = waveOutPrepareHeader(audioBlob->waveOutHdr, waveHeader, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    result = waveOutWrite(audioBlob->waveOutHdr, waveHeader, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    waveHeader->dwBufferLength = have;
    audioBlob->usedBytes += have;
  /* ... no more audio left to buffer */
  } else {
    if (audioBlob->numBuffers > 0) { 
      PyMem_Free(waveHeader->lpData);
      PyMem_Free(waveHeader);
      audioBlob->numBuffers--; 
    } else {
      /* all done, cleanup */
      waveOutClose(audioBlob->waveOutHdr);
      destroyAudioBlob(audioBlob);
      /* admitted, this is terrible */
      return MMSYSERR_NOERROR - 1;
    }
  }
  
  return MMSYSERR_NOERROR;
}

DWORD WINAPI bufferThread(LPVOID threadParam) {
  winAudioBlob_t* audioBlob = (winAudioBlob_t*)threadParam;
  MSG message;
  WAVEHDR* waveHeader;
  MMRESULT result;
    
  /* wait for the "audio block done" message" */
  while (1) {
    GetMessage(&message, NULL, 0, 0);
    if (message.message == MM_WOM_DONE) {
      waveHeader = (WAVEHDR*)message.lParam;
      result = fillBuffer(waveHeader, audioBlob);
      if (result != MMSYSERR_NOERROR) {break;}
    }
    if (message.message == WM_QUIT) {break;}
  }

  return 0;
}

simpleaudioError_t playOS(void* audioData, int lenSamples, int numChannels, 
  int bitsPerChan, int sampleRate, playItem_t* playListHead) {
  winAudioBlob_t* audioBlob;
  WAVEFORMATEX audioFmt; 
  MMRESULT result;
  simpleaudioError_t error = {SIMPLEAUDIO_OK, 0, "", ""};
  HANDLE threadHandle = NULL;
  DWORD threadId;
  int bytesPerChan = bitsPerChan / 8;
  int bytesPerFrame = bytesPerChan * numChannels;
  WAVEHDR* tempWaveHeader;
  int i;
  
  /* initial allocation and audio buffer copy */
  audioBlob = createAudioBlob();
  audioBlob->audioBuffer = PyMem_Malloc(lenSamples * bytesPerFrame);
  memcpy(audioBlob->audioBuffer, audioData, lenSamples * bytesPerFrame);
  audioBlob->lenBytes = lenSamples * bytesPerFrame;
  audioBlob->usedBytes = 0;
  audioBlob->numBuffers = 0;
  
  /* setup the linked list item for this playback buffer */
  grabMutex(playListHead->mutex);
  audioBlob->playListItem = newListItem(playListHead);
  audioBlob->playListItem->playId = 0;
  audioBlob->playListItem->stopFlag = 0;
  releaseMutex(playListHead->mutex);
  
  /* windows audio device and format headers setup */
  audioFmt.wFormatTag = WAVE_FORMAT_PCM;
  audioFmt.nChannels = numChannels;
  
  audioFmt.nSamplesPerSec = sampleRate; 
  audioFmt.nBlockAlign = bytesPerFrame; 
  /* per MSDN WAVEFORMATEX documentation */
  audioFmt.nAvgBytesPerSec = audioFmt.nSamplesPerSec * audioFmt.nBlockAlign;
  audioFmt.wBitsPerSample = bitsPerChan;
  audioFmt.cbSize = 0;

  /* create the cleanup thread so we can return execution to TCL immediately
       after calling waveOutWrite 
  SEE :http://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx */
  threadHandle = CreateThread(NULL, 0, bufferThread, audioBlob, 0, &threadId);
  if (threadHandle == NULL) {
    DWORD lastError = GetLastError();
    /* lang code : US En */
    FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), 
        NULL, lastError, 0x0409, error.sysMessage, SA_ERR_STR_LEN, NULL);
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)lastError;
    strncpy(error.apiMessage, "Failed to open audio device.", SA_ERR_STR_LEN);
    
    destroyAudioBlob(audioBlob);
    return error;
  }
  
  /* open a handle to the default audio device */
  result = waveOutOpen(&audioBlob->waveOutHdr, WAVE_MAPPER, &audioFmt, threadId, 0, CALLBACK_THREAD);
  if (result != MMSYSERR_NOERROR) {
    error.errorState = SIMPLEAUDIO_ERROR;
    error.code = (simpleaudioCode_t)result;
    waveOutGetErrorText(result, error.sysMessage, SA_ERR_STR_LEN);
    strncpy(error.apiMessage, "Failed to open audio device.", SA_ERR_STR_LEN);

    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    destroyAudioBlob(audioBlob);
    return error;
  }
  
  /* fill and write two buffers */
  for (i = 0; i < 2; i++) {
    tempWaveHeader = PyMem_Malloc(sizeof(WAVEHDR));
    memset(tempWaveHeader, 0, sizeof(WAVEHDR));
    tempWaveHeader->lpData = PyMem_Malloc(SIMPLEAUDIO_BUFSZ);
    tempWaveHeader->dwBufferLength = SIMPLEAUDIO_BUFSZ;
    
    result = fillBuffer(tempWaveHeader, audioBlob);
    if (result != MMSYSERR_NOERROR) {
      error.errorState = SIMPLEAUDIO_ERROR;
      error.code = (simpleaudioCode_t)result;
      waveOutGetErrorText(result, error.sysMessage, SA_ERR_STR_LEN);
      strncpy(error.apiMessage, "Failed to buffer audio.", SA_ERR_STR_LEN);
      
      PostThreadMessage(threadId, WM_QUIT, 0, 0);
      waveOutUnprepareHeader(audioBlob->waveOutHdr, tempWaveHeader, sizeof(WAVEHDR));
      waveOutClose(audioBlob->waveOutHdr);
      destroyAudioBlob(audioBlob);
      return error;
    }
    
    audioBlob->numBuffers++;
  }
  
  return error;
}


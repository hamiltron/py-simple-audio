#ifndef SIMPLEAUDIO_H
#define SIMPLEAUDIO_H

#define SIMPLEAUDIO_ERRMSGLEN (256)
#define SIMPLEAUDIO_OK (0)
#define SIMPLEAUDIO_ERROR (1)
#define SIMPLEAUDIO_BUFSZ (1024)

#define CLEAR (0)
#define STOP (1)

enum {
  NOT_LAST_ITEM = 0,
  LAST_ITEM = 1
};

typedef unsigned long long simpleaudioCode_t;

/* structure used to collect and pass OS error information to interpreter */
typedef struct {
  int errorState;
  simpleaudioCode_t code;
  char sysMessage[SIMPLEAUDIO_ERRMSGLEN];
  char apiMessage[SIMPLEAUDIO_ERRMSGLEN];
} simpleaudioError_t;

/* linked list structure used to track the active playback items/threads */
typedef struct playItem_s {
  unsigned long long playId;
  int stopFlag;
  struct playItem_s* prevItem;
  struct playItem_s* nextItem;
  void* mutex;
} playItem_t;

/* prototypes */
simpleaudioError_t playOS(void* audioData, int lenSamples, int numChannels, 
    int bitsPerChan, int sampleRate, playItem_t* playListHead);
    
int deleteListItem(playItem_t* playItem);
playItem_t* newListItem(playItem_t* listHead);

void* create_mutex();
void destroy_mutex(void* mutex);
void grab_mutex(void* mutex);
void release_mutex(void* mutex);

#ifdef SIMPLEAUDIO_NOT_SUP
simpleaudioError_t playOS(void* audioData, int lenSamples, int numChannels, 
    int bitsPerChan, int sampleRate, playItem_t* playListHead) {
 simpleaudioError_t error = {SIMPLEAUDIO_OK, 0, "", ""};
 return error;
}
#endif

#endif /* SIMPLEAUDIO_H */
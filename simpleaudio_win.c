#include "simpleaudio.h"
#include <Windows.h>
#include <Mmsystem.h>
#include <stdlib.h>

#define SYS_STR_LEN (SA_ERR_STR_LEN / 2)

#define WIN_EXCEPTION(my_msg, code, err_msg, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d -- MSG: %s", my_msg, code, err_msg); \
    PyErr_SetString(sa_python_error, str_ptr);

enum {
  FILL_BUFFER_WRITE = 0,
  FILL_BUFFER_DRAINING = 1,
  FILL_BUFFER_DONE = 2
};

typedef struct {
  Py_buffer buffer_obj;
  HWAVEOUT wave_out_hdr;
  int used_bytes;
  int len_bytes;
  int num_buffers;
  play_item_t* play_list_item;
  void* list_mutex;
} win_audio_blob_t;

void destroy_audio_blob(win_audio_blob_t* audio_blob) {
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

MMRESULT fillBuffer(WAVEHDR* wave_header, win_audio_blob_t* audio_blob) {
  int want = wave_header->dwBufferLength;
  int have = audio_blob->len_bytes - audio_blob->used_bytes;
  int stop_flag = 0;
  MMRESULT result;

  grab_mutex(audio_blob->play_list_item->mutex);
  stop_flag = audio_blob->play_list_item->stop_flag;
  release_mutex(audio_blob->play_list_item->mutex);

  /* if there's still audio yet to buffer ... */
  if (have > 0 && !stop_flag) {
    if (have > want) {have = want;}
    result = waveOutUnprepareHeader(audio_blob->wave_out_hdr, wave_header, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    memcpy(wave_header->lpData, &((char*)audio_blob->buffer_obj.buf)[audio_blob->used_bytes], have);
    result = waveOutPrepareHeader(audio_blob->wave_out_hdr, wave_header, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    result = waveOutWrite(audio_blob->wave_out_hdr, wave_header, sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR) {return result;}
    wave_header->dwBufferLength = have;
    audio_blob->used_bytes += have;
  /* ... no more audio left to buffer */
  } else {
    if (audio_blob->num_buffers > 0) {
      PyMem_Free(wave_header->lpData);
      PyMem_Free(wave_header);
      audio_blob->num_buffers--;
    } else {
      /* all done, cleanup */
      waveOutClose(audio_blob->wave_out_hdr);
      destroy_audio_blob(audio_blob);
      /* admitted, this is terrible */
      return MMSYSERR_NOERROR - 1;
    }
  }

  return MMSYSERR_NOERROR;
}

DWORD WINAPI bufferThread(LPVOID threadParam) {
  win_audio_blob_t* audio_blob = (win_audio_blob_t*)threadParam;
  MSG message;
  WAVEHDR* wave_header;
  MMRESULT result;

  /* wait for the "audio block done" message" */
  while (1) {
    GetMessage(&message, NULL, 0, 0);
    if (message.message == MM_WOM_DONE) {
      wave_header = (WAVEHDR*)message.lParam;
      result = fillBuffer(wave_header, audio_blob);
      if (result != MMSYSERR_NOERROR) {break;}
    }
    if (message.message == WM_QUIT) {break;}
  }

  return 0;
}

PyObject* play_os(Py_buffer buffer_obj, int len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head) {
    char err_msg_buf[SA_ERR_STR_LEN];
    char sys_msg_buf[SA_ERR_STR_LEN / 2];
    win_audio_blob_t* audio_blob;
    WAVEFORMATEX audio_format;
    MMRESULT result;
    HANDLE thread_handle = NULL;
    DWORD thread_id;
    int bytes_per_frame = bytes_per_chan * num_channels;
    WAVEHDR* temp_wave_hdr;
    int i;

    DBG_PLAY_OS_CALL

    /* initial allocation and audio buffer copy */
    audio_blob = PyMem_Malloc(sizeof(win_audio_blob_t));

    DBG_CREATE_BLOB

    audio_blob->buffer_obj = buffer_obj;
    audio_blob->list_mutex = play_list_head->mutex;
    audio_blob->len_bytes = len_samples * bytes_per_frame;
    audio_blob->used_bytes = 0;
    audio_blob->num_buffers = 0;

    /* setup the linked list item for this playback buffer */
    grab_mutex(play_list_head->mutex);
    audio_blob->play_list_item = new_list_item(play_list_head);
    release_mutex(play_list_head->mutex);

    /* windows audio device and format headers setup */
    audio_format.wFormatTag = WAVE_FORMAT_PCM;
    audio_format.nChannels = num_channels;

    audio_format.nSamplesPerSec = sample_rate;
    audio_format.nBlockAlign = bytes_per_frame;
    /* per MSDN WAVEFORMATEX documentation */
    audio_format.nAvgBytesPerSec = audio_format.nSamplesPerSec * audio_format.nBlockAlign;
    audio_format.wBitsPerSample = bytes_per_chan * 8;
    audio_format.cbSize = 0;

    /* create the cleanup thread so we can return after calling waveOutWrite
       SEE :http://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx
    */
    thread_handle = CreateThread(NULL, 0, bufferThread, audio_blob, 0, &thread_id);
    if (thread_handle == NULL) {
        DWORD lastError = GetLastError();
        /* lang code : US En */
        FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS), NULL, lastError, 0x0409, sys_msg_buf, SYS_STR_LEN, NULL);
        WIN_EXCEPTION("Failed to start cleanup thread.", 0, sys_msg_buf, err_msg_buf);

        destroy_audio_blob(audio_blob);
        return NULL;
    }

    /* open a handle to the default audio device */
    result = waveOutOpen(&audio_blob->wave_out_hdr, WAVE_MAPPER, &audio_format, thread_id, 0, CALLBACK_THREAD);
    if (result != MMSYSERR_NOERROR) {
        waveOutGetErrorText(result, sys_msg_buf, SYS_STR_LEN);
        WIN_EXCEPTION("Failed to open audio device.", result, sys_msg_buf, err_msg_buf);

        PostThreadMessage(thread_id, WM_QUIT, 0, 0);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    /* fill and write two buffers */
    for (i = 0; i < 2; i++) {
        temp_wave_hdr = PyMem_Malloc(sizeof(WAVEHDR));
        memset(temp_wave_hdr, 0, sizeof(WAVEHDR));
        temp_wave_hdr->lpData = PyMem_Malloc(SIMPLEAUDIO_BUFSZ);
        temp_wave_hdr->dwBufferLength = SIMPLEAUDIO_BUFSZ;

        result = fillBuffer(temp_wave_hdr, audio_blob);
        if (result != MMSYSERR_NOERROR) {
            waveOutGetErrorText(result, sys_msg_buf, SYS_STR_LEN);
            WIN_EXCEPTION("Failed to buffer audio.", result, sys_msg_buf, err_msg_buf);

            PostThreadMessage(thread_id, WM_QUIT, 0, 0);
            waveOutUnprepareHeader(audio_blob->wave_out_hdr, temp_wave_hdr, sizeof(WAVEHDR));
            waveOutClose(audio_blob->wave_out_hdr);
            destroy_audio_blob(audio_blob);
            return NULL;
        }

        audio_blob->num_buffers++;
    }

    return PyLong_FromUnsignedLongLong(audio_blob->play_list_item->play_id);
}


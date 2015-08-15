#include "simpleaudio.h"
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <unistd.h>

#define ALSA_EXCEPTION(my_msg, code, err_msg, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d -- MSG: %s", my_msg, code, err_msg); \
    PyErr_SetString(PyExc_Exception, str_ptr);

#define RESAMPLE (1)
#define LATENCY_US (100000) /* 100 ms */

typedef struct {
    void* audio_buffer;
    snd_pcm_t* handle;
    int samples_left;
    int samples_played;
    int frame_size;
    play_item_t* play_list_item;
    void* list_mutex;
} alsa_audio_blob_t;

void destroy_audio_blob(alsa_audio_blob_t* audio_blob) {
    DBG_DESTROY_BLOB

    PyMem_Free(audio_blob->audio_buffer);
    grab_mutex(audio_blob->list_mutex);
    delete_list_item(audio_blob->play_list_item);
    release_mutex(audio_blob->list_mutex);
    PyMem_Free(audio_blob);
}

void* playback_thread(void* thread_param) {
    #if DEBUG > 0 
    fprintf(DBG_OUT, DBG_PRE"playback thread started with audio blob at %p\n", thread_param);
    #endif

    alsa_audio_blob_t* audio_blob = (alsa_audio_blob_t*)thread_param;
    void* audio_ptr;
    int play_samples;
    int result;
    int stop_flag = 0;

    while (audio_blob->samples_left > 0 && !stop_flag) {
        grab_mutex(audio_blob->play_list_item->mutex);
        stop_flag = audio_blob->play_list_item->stop_flag;
        release_mutex(audio_blob->play_list_item->mutex);

        #if DEBUG > 1
        fprintf(DBG_OUT, DBG_PRE"loop iteration with stop flag: %d\n", stop_flag);
        #endif

        if (audio_blob->samples_left < SIMPLEAUDIO_BUFSZ / audio_blob->frame_size) {
            play_samples = audio_blob->samples_left;
        } else {
            play_samples = SIMPLEAUDIO_BUFSZ / audio_blob->frame_size;
        }
        audio_ptr = audio_blob->audio_buffer + (size_t)(audio_blob->samples_played * audio_blob->frame_size);
        result = snd_pcm_writei(audio_blob->handle, audio_ptr, play_samples);
        if (result < 0) {
            result = snd_pcm_recover(audio_blob->handle, result, 0);
            if (result < 0) {
                /* unrecoverable error */
                break;
            } 
        } else {
            audio_blob->samples_played += result;
            audio_blob->samples_left -= result;
        }
    }

    #if DEBUG > 1
    fprintf(DBG_OUT, DBG_PRE"done buffering audio - cleaning up\n");
    #endif

    snd_pcm_drain(audio_blob->handle);
    snd_pcm_close(audio_blob->handle);
    destroy_audio_blob(audio_blob);

    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"playback_thread_done");
    #endif

    pthread_exit(0);
}

PyObject* play_os(void* audio_data, len_samples_t len_samples, int num_channels, int bytes_per_chan, int sample_rate, play_item_t* play_list_head) {
    DBG_PLAY_OS_CALL
    
    char err_msg_buf[SA_ERR_STR_LEN];
    alsa_audio_blob_t* audio_blob;
    int bytesPerFrame = bytes_per_chan * num_channels;
    static char *device = "default";
    snd_pcm_format_t sample_format;
    pthread_t play_thread;
    int result;

    /* set that format appropriately */
    if (bytes_per_chan == 2) {
        sample_format = SND_PCM_FORMAT_S16_LE;
    } else {
        ALSA_EXCEPTION("Unsupported Sample Format.", 0, "", err_msg_buf);
        return NULL;
    }

    /* audio blob initial allocation and audio buffer copy */
    audio_blob = PyMem_Malloc(sizeof(alsa_audio_blob_t));

    DBG_CREATE_BLOB

    audio_blob->list_mutex = play_list_head->mutex;
    audio_blob->handle = NULL;
    audio_blob->audio_buffer = PyMem_Malloc(len_samples * bytesPerFrame);
    memcpy(audio_blob->audio_buffer, audio_data, len_samples * bytesPerFrame);
    audio_blob->samples_left = len_samples;
    audio_blob->samples_played = 0;
    audio_blob->frame_size = bytesPerFrame;

    /* setup the linked list item for this playback buffer */
    grab_mutex(play_list_head->mutex);
    audio_blob->play_list_item = new_list_item(play_list_head);
    release_mutex(play_list_head->mutex);

    /* open access to a PCM device (blocking mode)  */
    result = snd_pcm_open(&audio_blob->handle, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (result < 0) {
        ALSA_EXCEPTION("Error opening PCM device.", result, snd_strerror(result), err_msg_buf);
        destroy_audio_blob(audio_blob);
        return NULL;
     }

    /* set the PCM params */
    result = snd_pcm_set_params(audio_blob->handle, sample_format, SND_PCM_ACCESS_RW_INTERLEAVED, num_channels, sample_rate, RESAMPLE, LATENCY_US);
    if (result < 0) {
        ALSA_EXCEPTION("Error setting parameters.", result, snd_strerror(result), err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    /* fire off the playback thread */
    result = pthread_create(&play_thread, NULL, playback_thread, (void*)audio_blob);
    if (result != 0) {
        ALSA_EXCEPTION("Could not create playback thread.", result, "", err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    return PyLong_FromUnsignedLongLong(audio_blob->play_list_item->play_id);
}


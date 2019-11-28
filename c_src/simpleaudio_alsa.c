/*
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton
MIT License (see LICENSE.txt)
*/

#include "simpleaudio.h"
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <unistd.h>

#define ALSA_EXCEPTION(my_msg, code, err_msg, str_ptr) \
    snprintf(str_ptr, SA_ERR_STR_LEN, "%s -- CODE: %d -- MSG: %s", my_msg, code, err_msg); \
    PyErr_SetString(sa_python_error, str_ptr);

#define RESAMPLE (1)

void* playback_thread(void* thread_param) {
    audio_blob_t* audio_blob = (audio_blob_t*)thread_param;
    void* audio_ptr;
    int play_samples;
    int samples_left = (audio_blob->len_bytes - audio_blob->used_bytes) / audio_blob->frame_size;
    int buffer_samples = audio_blob->buffer_size / audio_blob->frame_size;
    int result;
    int stop_flag = 0;

    dbg1("playback thread started with audio blob at %p\n", thread_param);

    while (samples_left > 0 && !stop_flag) {
        grab_mutex(audio_blob->play_list_item->mutex);
        stop_flag = audio_blob->play_list_item->stop_flag;
        release_mutex(audio_blob->play_list_item->mutex);

        dbg2("loop iteration with stop flag: %d\n", stop_flag);

        if (samples_left < audio_blob->buffer_size) {
            play_samples = samples_left;
        } else {
            play_samples = buffer_samples;
        }
        audio_ptr = audio_blob->buffer_obj.buf + (size_t)(audio_blob->used_bytes);
        result = snd_pcm_writei(audio_blob->handle, audio_ptr, play_samples);
        if (result < 0) {
            dbg2("snd_pcm_writei error code: %d\n", result);

            result = snd_pcm_recover(audio_blob->handle, result, 0);
            if (result < 0) {
                dbg2("unrecoverable error - code: %d\n", result);

                /* unrecoverable error */
                break;
            }
        } else {
            audio_blob->used_bytes += result * audio_blob->frame_size;
        }

        samples_left = (audio_blob->len_bytes - audio_blob->used_bytes) / audio_blob->frame_size;
    }

    dbg2("done buffering audio - cleaning up\n");

    snd_pcm_drain(audio_blob->handle);
    snd_pcm_close(audio_blob->handle);
    destroy_audio_blob(audio_blob);

    dbg1("playback thread done");

    pthread_exit(0);
}

PyObject* play_os(Py_buffer buffer_obj, int len_samples, int num_channels, int bytes_per_chan,
                  int sample_rate, play_item_t* play_list_head, int latency_us) {
    char err_msg_buf[SA_ERR_STR_LEN];
    audio_blob_t* audio_blob;
    int bytes_per_frame = bytes_per_chan * num_channels;
    static char *device = "default";
    snd_pcm_format_t sample_format;
    pthread_t play_thread;
    pthread_attr_t thread_attr;
    int result;
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_uframes_t buffer_frames;

    /* set detachable thread attribute */
    result = pthread_attr_init(&thread_attr);
    if (result != 0) {
        ALSA_EXCEPTION("Error initializing thread attributes.", result, "", err_msg_buf);
        return NULL;
    }
    result = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (result != 0) {
        ALSA_EXCEPTION("Error setting detachable thread attribute.", result, "", err_msg_buf);
        return NULL;
    }

    /* not sure where the best place to do this is or if it matters */
    snd_pcm_hw_params_alloca(&hw_params);

    DBG_PLAY_OS_CALL

    /* set that format appropriately */
    if (bytes_per_chan == 1) {
        sample_format = SND_PCM_FORMAT_U8;
    } else if (bytes_per_chan == 2) {
        sample_format = SND_PCM_FORMAT_S16_LE;
    } else if (bytes_per_chan == 3) {
        sample_format = SND_PCM_FORMAT_S24_3LE;
    } else if (bytes_per_chan == 4) {
        sample_format = SND_PCM_FORMAT_FLOAT_LE;
    } else {
        ALSA_EXCEPTION("Unsupported Sample Format.", 0, "", err_msg_buf);
        return NULL;
    }

    /* audio blob initial allocation and audio buffer copy */
    audio_blob = create_audio_blob();
    audio_blob->buffer_obj = buffer_obj;
    audio_blob->list_mutex = play_list_head->mutex;
    audio_blob->len_bytes = len_samples * bytes_per_frame;
    audio_blob->frame_size = bytes_per_frame;

    /* setup the linked list item for this playback buffer */
    grab_mutex(play_list_head->mutex);
    audio_blob->play_list_item = new_list_item(play_list_head);
    release_mutex(play_list_head->mutex);

    /* open access to a PCM device (blocking mode)  */
    result = snd_pcm_open((snd_pcm_t**)&audio_blob->handle, device, SND_PCM_STREAM_PLAYBACK, 0);
    if (result < 0) {
        ALSA_EXCEPTION("Error opening PCM device.", result, snd_strerror(result), err_msg_buf);
        destroy_audio_blob(audio_blob);
        return NULL;
     }

    /* set the PCM params using ALSA's convenience function */
    result = snd_pcm_set_params(audio_blob->handle, sample_format, SND_PCM_ACCESS_RW_INTERLEAVED,
                                num_channels, sample_rate, RESAMPLE, latency_us);
    if (result < 0) {
        ALSA_EXCEPTION("Error setting parameters.", result, snd_strerror(result), err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    /* get the HW params (needed for buffer size) */
    result = snd_pcm_hw_params_current(audio_blob->handle, hw_params);
    if (result < 0) {
        ALSA_EXCEPTION("Error getting hardware parameters.", result, snd_strerror(result), err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    /* get the buffer size */
    result = snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_frames);
    if (result < 0) {
        ALSA_EXCEPTION("Error getting buffer_size.", result, snd_strerror(result), err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }
    audio_blob->buffer_size = buffer_frames * bytes_per_chan * num_channels;

    dbg1("ALSA says buffer size is %d bytes\n", audio_blob->buffer_size);

    /* fire off the playback thread */
    result = pthread_create(&play_thread, &thread_attr, playback_thread, (void*)audio_blob);
    if (result != 0) {
        ALSA_EXCEPTION("Could not create playback thread.", result, "", err_msg_buf);
        snd_pcm_close(audio_blob->handle);
        destroy_audio_blob(audio_blob);
        return NULL;
    }

    return PyLong_FromUnsignedLongLong(audio_blob->play_list_item->play_id);
}

/*
Simpleaudio Python Extension
Copyright (C) 2015, Joe Hamilton
MIT License (see LICENSE.txt)
*/

#include "simpleaudio.h"

PyObject* sa_python_error;

play_item_t play_list_head = {
    0,         /* play_id */
    SA_CLEAR,  /* stop_flag */
    NULL,      /* prev_item */
    NULL,      /* next_item */
    NULL       /* mutex */
};

static PyObject* _stop(PyObject *self, PyObject *args)
{
    play_id_t play_id;
    play_item_t* list_item = play_list_head.next_item;

    dbg1("_stop call\n");

    if (!PyArg_ParseTuple(args, "K", &play_id)) {
        return NULL;
    }

    dbg1("looking for play ID %llu\n", play_id);

    /* walk the list and find the matching play ID */
    grab_mutex(play_list_head.mutex);
    while(list_item != NULL) {
        if (list_item->play_id == play_id) {
            #if DEBUG > 0
            fprintf(DBG_OUT, DBG_PRE"found play ID in list item at %p\n", list_item);
            #endif

            grab_mutex(list_item->mutex);
            list_item->stop_flag = SA_STOP;
            release_mutex(list_item->mutex);
            break;
        }
        list_item = list_item->next_item;
    }
    release_mutex(play_list_head.mutex);

    Py_RETURN_NONE;
}

static PyObject* _stop_all(PyObject *self, PyObject *args)
{
    play_item_t* list_item = play_list_head.next_item;

    dbg1("_stop_all call\n");

    /* walk the list and set all audio to stop */
    grab_mutex(play_list_head.mutex);
    while(list_item != NULL) {
        dbg1("stopping ID %llu in list item at %p\n", list_item->play_id, list_item);

        grab_mutex(list_item->mutex);
        list_item->stop_flag = SA_STOP;
        release_mutex(list_item->mutex);
        list_item = list_item->next_item;
    }
    release_mutex(play_list_head.mutex);

    Py_RETURN_NONE;
}

static PyObject* _is_playing(PyObject *self, PyObject *args)
{
    play_id_t play_id;
    play_item_t* list_item = play_list_head.next_item;
    int found = 0;

    dbg1("_is_playing call\n");

    if (!PyArg_ParseTuple(args, "K", &play_id)) {
        return NULL;
    }

    dbg1("looking for play ID %llu\n", play_id);

    /* walk the list and find the matching play ID */
    grab_mutex(play_list_head.mutex);
    while(list_item != NULL) {
        if (list_item->play_id == play_id) {
            #if DEBUG > 0
            fprintf(DBG_OUT, DBG_PRE"found play ID in list item at %p\n", list_item);
            #endif

            found = 1;
        }
        list_item = list_item->next_item;
    }
    release_mutex(play_list_head.mutex);

    if (found) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* _play_buffer(PyObject *self, PyObject *args)
{
    PyObject* audio_obj;
    Py_buffer buffer_obj;
    int num_channels;
    int bytes_per_channel;
    int sample_rate;
    int num_samples;

    dbg1("_play_buffer call\n");

    if (!PyArg_ParseTuple(args, "Oiii", &audio_obj, &num_channels, &bytes_per_channel, &sample_rate)) {
        return NULL;
    }

    if (PyObject_GetBuffer(audio_obj, &buffer_obj, PyBUF_SIMPLE) == -1) {
        /* could not get buffer -PyObject_GetBuffer
           should have set the appropriate error
        */
        return NULL;
    }

    if (bytes_per_channel < 1 || bytes_per_channel > 2) {
        PyErr_SetString(PyExc_ValueError, "Bytes-per-sample must be 1 (8-bit) or 2 (16-bit).");
        return NULL;
    }

    if (num_channels < 1 || num_channels > 2) {
        PyErr_SetString(PyExc_ValueError, "Number of channels must be 1 or 2.");
        return NULL;
    }

    if (sample_rate != 8000 &&
            sample_rate != 11025 &&
            sample_rate != 16000 &&
            sample_rate != 22050 &&
            sample_rate != 24000 &&
            sample_rate != 32000 &&
            sample_rate != 44100 &&
            sample_rate != 48000 &&
            sample_rate != 88200 &&
            sample_rate != 96000 &&
            sample_rate != 192000) {
        PyErr_SetString(PyExc_ValueError, "Weird sample rates are not supported.");
        return NULL;
    }

    if (buffer_obj.len % (bytes_per_channel * num_channels) != 0) {
        PyErr_SetString(PyExc_ValueError, "Buffer size (in bytes) is not a multiple of bytes-per-sample and the number of channels.");
        return NULL;
    }
    num_samples = buffer_obj.len / bytes_per_channel / num_channels;

    /* explicitly tell Python we're using threading since the
       it requires a cross-thread API call to release the buffer
       view when we're done playing audio */
    PyEval_InitThreads();

    /* fixed 100ms latency */
    return play_os(buffer_obj, num_samples, num_channels, bytes_per_channel, sample_rate, &play_list_head, SA_LATENCY_US);
}

static PyMethodDef _simpleaudio_methods[] = {
    {"_play_buffer",  _play_buffer, METH_VARARGS, "Play audio from an object supporting the buffer interface."},
    {"_stop",  _stop, METH_VARARGS, "Stop playback of a specified audio object."},
    {"_stop_all",  _stop_all, METH_NOARGS, "Stop playback of all audio objects."},
    {"_is_playing",  _is_playing, METH_VARARGS, "Indicate whether the specified audio object is still playing."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static char doc_string [] = "_simpleaudio is the module containing the C-extension that handles the low-level OS-specific API interactions for audio playback.";

static struct PyModuleDef _simpleaudio_module = {
   PyModuleDef_HEAD_INIT,
   "_simpleaudio",   /* name of module */
   doc_string,       /* module documentation, may be NULL */
   -1,               /* size of per-interpreter state of the module,
                        or -1 if the module keeps state in global variables. */
   _simpleaudio_methods
};

PyMODINIT_FUNC
PyInit__simpleaudio(void)
{
    PyObject *m;

    m = PyModule_Create(&_simpleaudio_module);
    if (m == NULL)
        return NULL;

    sa_python_error = PyErr_NewException("_simpleaudio.SimpleaudioError", NULL, NULL);
    Py_INCREF(sa_python_error);
    PyModule_AddObject(m, "SimpleaudioError", sa_python_error);

    /* initialize the list head mutex */
    play_list_head.mutex = create_mutex();

    dbg1("init'd list head at %p\n", &play_list_head);

    return m;
}

/*********************************************/

void delete_list_item(play_item_t* play_item) {
    dbg1("deleting list item at %p with ID %llu between (prev) %p and (next) %p\n", play_item, play_item->play_id, play_item->prev_item, play_item->next_item);

    if (play_item->next_item != NULL) {
        play_item->next_item->prev_item = play_item->prev_item;
    }
    if (play_item->prev_item != NULL) {
        play_item->prev_item->next_item = play_item->next_item;
    }
    destroy_mutex(play_item->mutex);
    PyMem_Free(play_item);
}

/*********************************************/

play_item_t* new_list_item(play_item_t* list_head) {
    play_item_t* new_item;
    play_item_t* old_tail;

    new_item = PyMem_Malloc(sizeof(play_item_t));
    new_item->next_item = NULL;

    old_tail = list_head;
    while(old_tail->next_item != NULL) {
        old_tail = old_tail->next_item;
    }
    old_tail->next_item = new_item;

    new_item->prev_item = old_tail;
    new_item->mutex = create_mutex();
    new_item->play_id = (list_head->play_id)++;
    new_item->stop_flag = SA_CLEAR;

    dbg1("new list item at %p with ID %llu attached to %p\n", new_item, new_item->play_id, old_tail);

    return new_item;
}

/********************************************/

int get_buffer_size(int latency_us, int sample_rate, int frame_size) {
    return (long long)latency_us * sample_rate / 1000000 * frame_size;
}

/********************************************/

void destroy_audio_blob(audio_blob_t* audio_blob) {
    PyGILState_STATE gstate;

    dbg1("destroying audio blob at %p\n", audio_blob);

    /* release the buffer view so Python can
       decrement it's reference count*/
    gstate = PyGILState_Ensure();
    PyBuffer_Release(&audio_blob->buffer_obj);
    PyGILState_Release(gstate);

    grab_mutex(audio_blob->list_mutex);
    delete_list_item(audio_blob->play_list_item);
    release_mutex(audio_blob->list_mutex);
    PyMem_Free(audio_blob);
}

/********************************************/

audio_blob_t* create_audio_blob() {
    audio_blob_t* audio_blob = PyMem_Malloc(sizeof(audio_blob_t));

    dbg1("created audio blob at %p\n", audio_blob);

    memset(audio_blob, 0, sizeof(audio_blob_t));

    return audio_blob;
}

/********************************************/

void dbg1(const char* str, ...) {
#if DEBUG > 0
    va_list args;

    va_start(args, str);
    fprintf(DBG_OUT, DBG_PRE);
    vfprintf(DBG_OUT, str, args);
    va_end(args);
#endif
}

void dbg2(const char* str, ...) {
#if DEBUG > 1
    va_list args;

    va_start(args, str);
    fprintf(DBG_OUT, DBG_PRE);
    vfprintf(DBG_OUT, str, args);
    va_end(args);
#endif
}

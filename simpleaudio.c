#include <Python.h>
#include <stdlib.h>
#include "simpleaudio.h"

#include <stdio.h>

playItem_t play_list_head = {
    .playId = 0, 
    .stopFlag = CLEAR,
    .prevItem = NULL,
    .nextItem = NULL,
    .mutex = NULL};

static PyObject *
play_buffer(PyObject *self, PyObject *args)
{
    PyObject* audio_obj;
    Py_buffer audio_buffer;
    int result;
    simpleaudioError_t play_error;
    char error_str[SIMPLEAUDIO_ERRMSGLEN  * 4];
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int sample_rate;
    unsigned long long num_samples;

    if (!PyArg_ParseTuple(args, "OIII", &audio_obj, &num_channels, &bytes_per_sample, &sample_rate)) {
        return NULL;
    }
    
    result = PyObject_GetBuffer(audio_obj, &audio_buffer, PyBUF_SIMPLE);
    
    if (result != -1) {
        num_samples = audio_buffer.len / bytes_per_sample / num_channels;
        printf("%d samples\n", (int)num_samples);

        play_error = playOS(audio_buffer.buf, num_samples, num_channels, bytes_per_sample * 8, sample_rate, &play_list_head);
        if (play_error.errorState) {
            snprintf(error_str, (SIMPLEAUDIO_ERRMSGLEN  * 4), "Error: %s\n  syserr: %s\n  code: %llX\n", play_error.apiMessage, play_error.sysMessage, play_error.code);
            PyErr_SetString(PyExc_Exception, error_str);
            return NULL;
        }
        
        PyBuffer_Release(&audio_buffer);
    } 
    
    Py_RETURN_NONE; 
}

static PyMethodDef _simpleaudio_methods[] = {
    {"play_buffer",  play_buffer, METH_VARARGS,
     "Play audio from a buffer."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static char doc_string [] = "simpleaudio is a module that makes playing audio in Python very simple.";

static struct PyModuleDef _simpleaudio_module = {
   PyModuleDef_HEAD_INIT,
   "_simpleaudio",   /* name of module */
   doc_string, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   _simpleaudio_methods
};

static PyObject *AudioError;

PyMODINIT_FUNC
PyInit__simpleaudio(void)
{
    PyObject *m;

    m = PyModule_Create(&_simpleaudio_module);
    if (m == NULL)
        return NULL;

    AudioError = PyErr_NewException("simpleaudio.error", NULL, NULL);
    Py_INCREF(AudioError);
    PyModule_AddObject(m, "error", AudioError);
    
    /* initialize the list head mutex */
    play_list_head.mutex = create_mutex();
    
    return m;
}

/* shorter version of init function 
PyMODINIT_FUNC
PyInit_spam(void)
{
    return PyModule_Create(&spammodule);
}
*/

/*********************************************/

int deleteListItem(playItem_t* playItem) {
  int lastItemStatus = LAST_ITEM;
  if (playItem->nextItem != NULL) {
    playItem->nextItem->prevItem = playItem->prevItem;
    lastItemStatus = NOT_LAST_ITEM;
  } 
  if (playItem->prevItem != NULL) {
    playItem->prevItem->nextItem = playItem->nextItem;
    lastItemStatus = NOT_LAST_ITEM;
  }
  free(playItem);
  return lastItemStatus;
}

playItem_t* newListItem(playItem_t* listHead) {
  playItem_t* newItem;
  playItem_t* oldTail;
  
  newItem = malloc(sizeof(playItem_t));
  newItem->nextItem = NULL;
  
  oldTail = listHead;
  while(oldTail->nextItem != NULL) {
    oldTail = oldTail->nextItem;
  }  
  oldTail->nextItem = newItem;
  newItem->prevItem = oldTail;
  newItem->mutex = listHead->mutex;
  
  return newItem;
}


/********************************************/
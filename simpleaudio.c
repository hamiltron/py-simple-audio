#include "simpleaudio.h"

playItem_t play_list_head = {
    .playId = 0, 
    .stopFlag = SA_CLEAR,
    .prevItem = NULL,
    .nextItem = NULL,
    .mutex = NULL};

static PyObject* play_buffer(PyObject *self, PyObject *args)
{
    PyObject* audio_obj;
    Py_buffer audio_buffer;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int sample_rate;
    len_samples_t num_samples;

    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"play_buffer call\n");
    #endif

    if (!PyArg_ParseTuple(args, "OIII", &audio_obj, &num_channels, &bytes_per_sample, &sample_rate)) {
        return NULL;
    }
    
    if (PyObject_GetBuffer(audio_obj, &audio_buffer, PyBUF_SIMPLE) == -1) {
        /* could not get buffer -PyObject_GetBuffer 
           should have set the appropriate error
        */
        return NULL;
    }

    num_samples = audio_buffer.len / bytes_per_sample / num_channels;
    return play_os(audio_buffer.buf, num_samples, num_channels, bytes_per_sample, sample_rate, &play_list_head);
}

static PyMethodDef _simpleaudio_methods[] = {
    {"play_buffer",  play_buffer, METH_VARARGS,
     "Play audio from a buffer."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static char doc_string [] = "_simpleaudio is the module containing the C-extension that actually does all the work.";

static struct PyModuleDef _simpleaudio_module = {
   PyModuleDef_HEAD_INIT,
   "_simpleaudio",   /* name of module */
   doc_string,       /* module documentation, may be NULL */
   -1,               /* size of per-interpreter state of the module,
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
    
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"init'd list head at %p\n", &play_list_head);
    #endif
    
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

void deleteListItem(playItem_t* playItem) {
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"deleting list item at %p with ID %llu between (prev) %p and (next) %p\n", playItem, playItem->playId, playItem->prevItem, playItem->nextItem);
    #endif
    
    if (playItem->nextItem != NULL) {
        playItem->nextItem->prevItem = playItem->prevItem;
    } 
    if (playItem->prevItem != NULL) {
        playItem->prevItem->nextItem = playItem->nextItem;
    }
    destroy_mutex(playItem->mutex);
    PyMem_Free(playItem);
}

/*********************************************/

playItem_t* newListItem(playItem_t* listHead) {
    playItem_t* newItem;
    playItem_t* oldTail;
    
    newItem = PyMem_Malloc(sizeof(playItem_t));
    newItem->nextItem = NULL;
    
    oldTail = listHead;
    while(oldTail->nextItem != NULL) {
        oldTail = oldTail->nextItem;
    }  
    oldTail->nextItem = newItem;
    
    newItem->prevItem = oldTail;
    newItem->mutex = create_mutex();
    newItem->playId = (listHead->playId)++;
    newItem->stopFlag = SA_CLEAR;
    
    #ifdef DEBUG
    fprintf(DBG_OUT, DBG_PRE"new list item at %p with ID %llu attached to %p\n", newItem, newItem->playId, oldTail);
    #endif
    
    return newItem;
}

/********************************************/

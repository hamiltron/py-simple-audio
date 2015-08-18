#include "simpleaudio.h"

PyObject* sa_python_error;

play_item_t play_list_head = {
    .play_id = 0,
    .stop_flag = SA_CLEAR,
    .prev_item = NULL,
    .next_item = NULL,
    .mutex = NULL};

static PyObject* play_buffer(PyObject *self, PyObject *args)
{
    PyObject* audio_obj;
    Py_buffer buffer_obj;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int sample_rate;
    len_samples_t num_samples;

    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"play_buffer call\n");
    #endif

    if (!PyArg_ParseTuple(args, "OIII", &audio_obj, &num_channels, &bytes_per_sample, &sample_rate)) {
        return NULL;
    }

    if (PyObject_GetBuffer(audio_obj, &buffer_obj, PyBUF_SIMPLE) == -1) {
        /* could not get buffer -PyObject_GetBuffer
           should have set the appropriate error
        */
        return NULL;
    }


    if (bytes_per_sample < 1 || bytes_per_sample > 3) {
        PyErr_SetString(PyExc_ValueError, "Bytes-per-sample must be 1 (8-bit), 2 (16-bit), or 3 (24-bit).");
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
            sample_rate != 32000 &&
            sample_rate != 44100 &&
            sample_rate != 48000 &&
            sample_rate != 88200 &&
            sample_rate != 96000 &&
            sample_rate != 192000) {
        PyErr_SetString(PyExc_ValueError, "Sample rate must be a standard sample rate.");
        return NULL;
    }

    num_samples = buffer_obj.len / bytes_per_sample / num_channels;

    /* explicitly tell Python we're using threading since the
       it requires a cross-thread API call to release the buffer
       view when we're done playing audio */
    PyEval_InitThreads();

    return play_os(buffer_obj, num_samples, num_channels, bytes_per_sample, sample_rate, &play_list_head);
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

void delete_list_item(play_item_t* play_item) {
    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"deleting list item at %p with ID %llu between (prev) %p and (next) %p\n", play_item, play_item->play_id, play_item->prev_item, play_item->next_item);
    #endif

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
    play_item_t* newItem;
    play_item_t* oldTail;

    newItem = PyMem_Malloc(sizeof(play_item_t));
    newItem->next_item = NULL;

    oldTail = list_head;
    while(oldTail->next_item != NULL) {
        oldTail = oldTail->next_item;
    }
    oldTail->next_item = newItem;

    newItem->prev_item = oldTail;
    newItem->mutex = create_mutex();
    newItem->play_id = (list_head->play_id)++;
    newItem->stop_flag = SA_CLEAR;

    #if DEBUG > 0
    fprintf(DBG_OUT, DBG_PRE"new list item at %p with ID %llu attached to %p\n", newItem, newItem->play_id, oldTail);
    #endif

    return newItem;
}

/********************************************/

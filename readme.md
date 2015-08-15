## TODO

* use Python memory allocation instead of stdlib
* convert from camel case
* add parameter validation
* hook into module shutdown to stop all audio
* add 'unsupported' dummy version of the function for Linux w/out ALSA
* add `PyBuffer_Release(&audio_buffer);`  to audio-finished cleanup
* add stop module method
* add stop_all module method
* add is_still_playing module method
* add intelligent buffer sizing

## platform changes

* change playOS to play_os
* change bitsPerChan to BytesPerChan
* return a PyObject* of the playback ID
* throw Python errors directly
* use Python mem mgmt
* play_os doesnt need to set play_id or stop_flag
* keep reference to list mutex in audio blobs
* linked list head is never deallocated - delete_list_item returns void
* change len_samples type
* remove create_audio_blob 
* add debug prints

## investigate

* how to check for underruns in OSX and Windows
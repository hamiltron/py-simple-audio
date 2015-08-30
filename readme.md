## TODO

* hook into module shutdown to stop all audio
* add 'unsupported' dummy version of the function for Linux w/out ALSA
* add stop module method
* add stop_all module method
* add is_still_playing module method
## investigate

* test what happens when the audio is short enough that only 1 of the buffers is needed on OSX and WIN
* look into alsa and windows handle release on playback errors
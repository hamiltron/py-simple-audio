Short Tutorial
==============

Import
------

Import the :mod:`simpleaudio` module::

   import simpleaudio as sa

Playing audio directly
----------------------

The simplest way to play audio is with :func:`~simpleaudio.play_buffer`. The
``audio_data`` parameter must be an object which supports the buffer interface.
(:keyword:`bytes` objects, Python arrays, and Numpy arrays all qualify.)::

   play_obj = sa.play_buffer(audio_data, 2, 2, 44100)

The `play_obj` object is an instance of :class:`~simpleaudio.PlayObject` 
which could be viewed as a 'handle' to the audio playback initiated by the
:func:`~simpleaudio.play_buffer` call. This can be used to stop playback 
of the audio clip::

   play_obj.stop()

It can used to check whether a sound clip is still playing::

   if play_obj.is_playing():
      print("still playing")

It can also be used to wait for the audio playback to finish. This is espcially
useful when a script or program would otherwise exit before playback is done
(stopping the playback thread and consequently the audio)::

   play_obj.wait_done()
   # script exit


:class:`~simpleaudio.WaveObject` 's
-----------------------------------

In order to facilitate cleaner code, the :class:`~simpleaudio.WaveObject` 
class is provided which stores a reference to the object containing the 
audio as well as a copy of the playback parameters. These can be instantiated
like so::

   wave_obj = sa.WaveObject(audio_data, 2, 2, 44100)

Playback is started with :meth:`~simpleaudio.WaveObject.play` and a 
:class:`~simpleaudio.PlayObject` is returned as 
with :func:`~simpleaudio.play_buffer`::

   play_obj = wave_obj.play()

A class method exists in order to conveniently create WaveObject instances
directly from WAV files on disk::

   wave_obj = sa.WaveObject.from_wave_file(path_to_file)

Similarly, instances can be created from Wave_read objects returned from
:func:`wave.open` from the Python standard library::

   wave_read = wave.open(path_to_file, 'rb')
   wave_obj = sa.WaveObject.from_wave_read(wave_read)

Using Numpy
-----------

TO DO




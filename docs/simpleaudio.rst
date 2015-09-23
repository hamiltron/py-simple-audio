:mod:`simpleaudio`
==================

.. module:: simpleaudio

Examples
--------

API
---

.. class:: WaveObject(audio_data, num_channels=2, bytes_per_sample=2, sample_rate=44100)

   Instances of :class:`WaveObject` represent pieces of audio ready for playback. 
   It encapsulates the audio data buffer, playback parameters (such as sample rate), 
   and provides a method to initiate playback. 

   :param audio_data: object with audio data (must support the buffer interface)
   :param int num_channels: the number of audio channels
   :param int bytes_per_sample: the number of bytes per single-channel sample
   :param int sample_rate: the sample rate in Hz

.. method:: WaveObject.play()

   Starts playback of the audio
   
   :rtype: :rtype: a :class:`PlayObject` instance for the playback job

.. classmethod:: WaveObject.from_wave_file(wave_file)

   Creates a WaveObject from a wave file on disk.

   :param str wave_file: a path to a wave file

.. classmethod:: WaveObject.from_wave_read(wave_read)

   Creates a WaveObject from a :class:`Wave_read` object as returned by 
   :meth:`wave.open()`. 

   :param wave_read: a :class:`Wave_read` object

.. class:: PlayObject

   Instances of :class:`PlayObject` are returned by :meth:`WaveObject.play()` and 
   :func:`play_buffer()` and are essentially handles to the 
   audio playback jobs initiated and allow basic actions to be taken on the job 
   (such as stopping playback).

.. method:: PlayObject.stop()

   Stops the playback job.

.. method:: PlayObject.is_playing()

   Returns a :keyword:`True` if the playback job is still running or
   :keyword:`False` if it has finished.
   
   :rtype: bool

.. method:: PlayObject.wait_done()

   Waits for the playback job to finish before returning.

.. function:: stop_all()

   Stop all currently playing audio.

.. function:: play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate)

   Start playback of audio data from an object supporting the buffer 
   interface and with the given playback parameters.

   :param audio_data: object with audio data (must support the buffer interface)
   :param int num_channels: the number of audio channels
   :param int bytes_per_sample: the number of bytes per single-channel sample
   :param int sample_rate: the sample rate in Hz
   :rtype: a :class:`PlayObject` instance for the playback job
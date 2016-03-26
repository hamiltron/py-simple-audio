:mod:`simpleaudio`
==================

.. module:: simpleaudio

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

   Returns true if the playback job is still running or
   false if it has finished.

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

Examples
--------

Playing a file::

   import simpleaudio as sa

   wave_obj = sa.WaveObject.from_wave_file(path_to_file)
   play_obj = wave_obj.play()
   play_obj.wait_done()

Playing a Wave_read object::

   import simpleaudio as sa
   import wave

   wave_read = wave.open(path_to_file, 'rb')
   wave_obj = sa.WaveObject.from_wave_read(wave_read)
   play_obj = wave_obj.play()
   play_obj.wait_done()

Playing an object supporting the buffer interface::

   import simpleaudio as sa
   import wave

   wave_read = wave.open(path_to_file, 'rb')
   audio_data = wave_read.readframes(wave_read.getnframes())
   num_channels = wave_read.getnchannels()
   bytes_per_sample = wave_read.getsampwidth()
   sample_rate = wave_read.getframerate()

   wave_obj = sa.WaveObject(audio_data, num_channels, bytes_per_sample, sample_rate)
   play_obj = wave_obj.play()
   play_obj.wait_done()

Play an object directly (without creating a :class:`WaveObject`)::

   import simpleaudio as sa
   import wave

   wave_read = wave.open(path_to_file, 'rb')
   audio_data = wave_read.readframes(wave_read.getnframes())
   num_channels = wave_read.getnchannels()
   bytes_per_sample = wave_read.getsampwidth()
   sample_rate = wave_read.getframerate()

   play_obj = sa.play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate)
   play_obj.wait_done()

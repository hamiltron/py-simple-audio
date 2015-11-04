Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on OSX, Windows, and Linux. 

MIT Licensed.

Quick-Start
-----------

Install with::

   pip3 install simpleaudio

   (see documentation for Linux installation)

Simple Example::

   import simpleaudio as sa
   
   wave_obj = sa.WaveObject.from_wave_file("path/to/file.wav")
   play_obj = wave_obj.play()
   play_obj.wait_done()

`Documentation at RTD <http://simpleaudio.readthedocs.org/>`_
=============================================================




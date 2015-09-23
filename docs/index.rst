Simpleaudio Package
===================

The simplaudio package provides cross-platform audio playback
capability for Python 3 with a very simple interface. OSX, Windows, and Linux
with ALSA are supported (see supported versions below for details).
It is MIT Licensed.

Quick-Start
-----------

Install with::

   pip3 install simpleaudio

Simple Example::

   import simpleaudio as sa
   
   wave_obj = sa.WaveObject.from_wave_file(path_to_file)
   play_obj = wave_obj.play()
   play_obj.wait_done()

.. toctree::
   :maxdepth: 2

   overview
   simpleaudio
   functionchecks

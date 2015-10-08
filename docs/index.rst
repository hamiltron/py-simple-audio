Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on OSX, Windows, and Linux. MIT Licensed.

Quick-Start
-----------

Install with::

   pip3 install simpleaudio

Simple Example::

   import simpleaudio as sa
   
   wave_obj = sa.WaveObject.from_wave_file(path_to_file)
   play_obj = wave_obj.play()
   play_obj.wait_done()

Contents
--------

.. toctree::
   :maxdepth: 2

   overview
   tutorial
   simpleaudio
   functionchecks

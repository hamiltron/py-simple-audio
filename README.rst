Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on OSX, Windows, and Linux.

MIT Licensed.

`Documentation at RTD <http://simpleaudio.readthedocs.org/>`_
-------------------------------------------------------------

Installation
------------

Installation (make sure the ``pip`` command is the right one for
your platform and Python version)::

   pip install simpleaudio

See documentation for additional installation information.

Simple Example
--------------

..code-block:: python

   import simpleaudio as sa

   wave_obj = sa.WaveObject.from_wave_file("path/to/file.wav")
   play_obj = wave_obj.play()
   play_obj.wait_done()




Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on macOS, Windows, and Linux.

MIT Licensed.

:ref:`installation-ref`
-----------------------

Quick Function Check
--------------------

.. code-block:: python

   import simpleaudio.functionchecks as fc

   fc.LeftRightCheck.run()

More on :ref:`function-checks-ref`.

Simple Example
--------------

.. code-block:: python

   import simpleaudio as sa

   wave_obj = sa.WaveObject.from_wave_file("path/to/file.wav")
   play_obj = wave_obj.play()
   play_obj.wait_done()

Contents
--------

.. toctree::
   :maxdepth: 2

   installation
   capabilities
   tutorial
   simpleaudio
   functionchecks
   releases

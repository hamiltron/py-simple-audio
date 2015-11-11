Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on OSX, Windows, and Linux.

MIT Licensed.

`Documentation at RTFD <http://simpleaudio.readthedocs.org/>`_
--------------------------------------------------------------

Installation
------------

Installation (make sure the ``pip`` command is the right one for
your platform and Python version)::

   pip install simpleaudio

See documentation for additional installation information.

Quick Function Check
--------------------

.. code-block:: python

   import simpleaudio.functionchecks as fc

   fc.LeftRightCheck.run()

See documentation for more on function checks.

Simple Example
--------------

.. code-block:: python

   import simpleaudio as sa

   wave_obj = sa.WaveObject.from_wave_file("path/to/file.wav")
   play_obj = wave_obj.play()
   play_obj.wait_done()

Support
-------

For usage and how-to questions, post a question on 
`StackOverflow <http://stackoverflow.com/>`_ 
and tag it 'pysimpleaudio'. 

For bug reports, please email simpleaudio.bugs@gmail.com.

Release Notes
-------------

* 1.0.0
  + Initial Release




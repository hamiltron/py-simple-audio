NOTE: THIS PROJECT IS ARCHIVED
==============================

I no longer have the time to dedicate to maintaining Simpleaudio (which at this point is mostly keeping CI builds working). 
This project should be considered archived, meaning that there will be no further updates or builds. 
For anyone seriously interested in taking over the project, I would be willing to help in that process. 
I would also be willing to revisit maintaining the library should anyone wish to sponsor it. 

Simpleaudio Package
===================

The simplaudio package provides cross-platform, dependency-free audio playback
capability for Python 3 on OSX, Windows, and Linux.

MIT Licensed.

`Documentation at RTFD <http://simpleaudio.readthedocs.io/>`_
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


Using `read_ and set_ratio`:

.. code-block:: python

   import simpleaudio as sa

   wave_obj = sa.WaveObject.from_wave_file("path/to/file.wav")
   play_obj = wave_obj.play()
   time.sleep(1)
   print("This is the propertion of the file already played", play_obj.read_ratio())
   play_obj.set_ratio(0.9)
   time.sleep(0.5)  # need to wait a bit for this change to be effective
   print("set_ratio moved the file playing towards the end of the file. Now at", play_obj.read_ratio())
   

Support
-------

For usage and how-to questions, first checkout the tutorial in the
documentation. If you're still stuck, post a question on
`StackOverflow <http://stackoverflow.com/>`_
and **tag it 'pysimpleaudio'**.

For bug reports, please create an
`issue on Github <https://github.com/hamiltron/py-simple-audio/issues>`_
.

Big Thanks To ...
-----------------

Jonas Kalderstam

Christophe Gohlke

Tom Christie

Many others for their contributions, documentation, examples, and more.

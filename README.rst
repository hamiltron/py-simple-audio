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

For usage and how-to questions, first checkout the tutorial in the
documentation. If you're still stuck, post a question on
`StackOverflow <http://stackoverflow.com/>`_
and **tag it 'pysimpleaudio'**.

For bug reports, please create an
`issue on Github <https://github.com/hamiltron/py-simple-audio/issues>`_
or email simpleaudio.bugs@gmail.com.

Big Thanks To ...
-----------------

Jonas Kalderstam for
`his explanation <http://cowboyprogrammer.org/building-python-wheels-for-windows/>`_
of how to get Windows 64-bit builds working.

Christophe Gohlke for his help getting Windows builds working for 3.5.

Tom Christie for his extensive Python package
`distribution tutorial <https://tom-christie.github.io/articles/pypi/>`_.

Many others for their contributions, documentation, examples, and more.

Release Notes
-------------

* 1.0.1

   * Fixed OSX and Linux 8-bit playback

* 1.0.0

   * Initial Release

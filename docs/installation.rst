.. _installation-ref:

Installation
============

Make sure you have pip installed. For Linux, this is
usually done with the distro package manager (example:
``sudo apt-get install python3-pip``). For Windows and macOS, have a look at
the `pip documentation <http://pip.readthedocs.org>`_.

.. note::

   The actual ``pip`` command may vary between platforms and Python versions.
   Substitute the correct one for usages in these examples. Some common
   variants for Python 3.x are: ``pip3``, ``pip3.x``, and ``pip-3.x``.

   Also, in some cases you may need to manually add pip's location
   to the 'path' environment variable.

Upgrade pip and setuptools::

   pip install --upgrade pip setuptools

Install with::

   pip install simpleaudio

Linux Dependencies
------------------

The Python 3 and ALSA development packages are required for pip to build
the extension. For Debian variants (including Raspbian),
this will usually get the job done::

   sudo apt-get install -y python3-dev libasound2-dev

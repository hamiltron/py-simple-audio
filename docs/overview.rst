Capabilities
============

Asynchronous Interface
----------------------

The module implements an asynchronous interface, meaning that program
execution continues immediately after audio playback is started and a background
thread takes care of the rest. This makes it easy to incorporate audio playback
into GUI-driven applications that need to remain responsive. This also means that
real-time audio applications (such as a synthesizer) are not possible since the entire
audio clip to be played must be ready before playback.

Supported Platforms
-------------------

* Windows: 7 or newer
* OSX: 10.7 (Lion) or newer
* Linux: should support any distro with a working ALSA implementation

Audio Formats
-------------

Simpleaudio supports standard signed-integer, little-endian formats - basically
what is usually contained in a **plain WAV file**.

Mono (1-channel) and stereo (2-channel) audio is supported.

The following sample rates are allowed (though not necessarily gauranteed
to be supported on your platform/hardware): 8, 11.025, 16, 22.05, 32, 44.1, 
48, 88.2, 96, and 192 kHz.

Installation
============

Install with::

   pip3 install simpleaudio

Linux
-----

First PIP for Python 3 is probably not installed by default. For Debian 
variants use::

sudo apt-get install python3-pip

The exact command to invoke PIP can vary and may be something like `pip-3.3` 
(for Python 3.3). 

The Python 3 and ALSA development packages are required for pip to build
the extension. For Debian variants, this will usually get the job done::

   sudo apt-get install -y python3-dev asound2-dev



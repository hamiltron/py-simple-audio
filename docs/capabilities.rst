Capabilities & Support
======================

Compatibility
-------------

Platforms
#########

* Windows: 7 or newer
* OSX: 10.6 (Snow Leopard) or newer
* Linux: should support any distro with a working ALSA implementation

Python
######

Python 3.3 and up is officially supported and wheels (with pre-built binaries)
are available for
Windows and OSX. Python 3.2 and earlier may work as well when
building the extension from source (as on Linux) and it has been
confirmed to work with Python 3.2 on the Raspberry Pi.

Asynchronous Interface
----------------------

The module implements an asynchronous interface, meaning that program
execution continues immediately after audio playback is started and a
background thread takes care of the rest. This makes it easy to incorporate
audio playback into GUI-driven applications that need to remain responsive.
This also means that real-time audio applications (such as a synthesizer) are
not possible since the entire audio clip to be played must be ready
before playback.

Audio Formats
-------------

Simpleaudio supports standard integer PCM formats - basically
what is usually contained in a **plain WAV file**.

Mono (1-channel) and stereo (2-channel) audio is supported.

The following sample rates are allowed (though not necessarily gauranteed
to be supported on your platform/hardware): 8, 11.025, 16, 22.05, 32, 44.1,
48, 88.2, 96, and 192 kHz.

Comparison to PyAudio
---------------------

`PyAudio <https://pypi.python.org/pypi/PyAudio>`_ is another cross-platform
audio library for Python. While it has more capability than simpleaudio,
such as recording and continuous audio streaming, it depends on having
`PortAudio <http://www.portaudio.com/>`_ which makes for a more complicated
installation. Simpleaudio is intended to be one-stop shopping for a
cross-platform audio interface intended for sound clip playback.

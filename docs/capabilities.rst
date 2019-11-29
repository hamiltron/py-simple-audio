Capabilities & Support
======================

Compatibility
-------------

Platforms
#########

* Windows: 7 or newer
* macOS: 10.6 (Snow Leopard) or newer
* Linux: should support any distro with a working ALSA implementation

Python
######

Python 3.7 and up is officially supported and wheels (with pre-built binaries)
are available for Windows and macOS. Earlier versions of Python may work as well when
building the extension from source (as on Linux).

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

Simpleaudio supports standard integer 8-, 16-, and 24-bit integer formats, as well as 32-bit floating point.
8-bit is unsigned. 16-bit and 24-bit are signed little-endian. 24-bit is three bytes per sample packed - i.e. each
sample is three bytes followed immediately by the next three-byte sample, *without* any padding byte to align
to a 4-byte word boundary). 32-bit is little-endian floating point.

Mono (1-channel) and stereo (2-channel) audio is supported.

The following sample rates are allowed (though not necessarily guaranteed
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

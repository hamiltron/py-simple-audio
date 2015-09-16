Simpleaudio Module Description
==============================

Overview
--------

The simplaudio module provides cross-platform audio playback
capability for Python 3 with a very simple interface. OSX, Windows, and Linux 
with ALSA are supported (see supported versions below for details). 
It is MIT Licensed.

Asynchronous Interface
----------------------

The module implements an asynchronous interface, meaning that program
execution continues immediately after audio playback is started and a background 
thread takes care of the rest. This makes it easy to incorporate audio playback
into GUI-driven applications that need to remain responsive. This also means that 
real-time audio applications (such as a synthesizer) are not possible since the entire
audio clip to be played must be ready. 

Supported Platforms
-------------------

* Windows
    * 7
    * 8.1 (not verified)
    * 10 (not verified)
    
* OSX
    * 10.3 - 10.8 (not verified)
    * 10.9
    * 10.10 (not verified)
    
* Linux - should support any distro with a working ALSA implementation

Audio Formats
-------------

Simpleaudio supports standard signed-integer, little-endian formats - basically 
what is usually contained in a **plain WAV file**. 

Mono (1-channel) and stereo (2-channel) audio is supported. 

The following sample rates (in Hz) are allowed, though not necessarily gauranteed 
to be supported on your platform/hardware: 8, 11.025, 16, 22.05, 32, 44.1, 48, 88.2, 96, and 192 kHz.

Installation
============

Install with::

    pip3 install simpleaudio

Linux Dependencies
------------------

The Python 3 and ALSA development packages are required for pip to build
the extension. For Debian, Ubuntu, and Raspbian, 
this will usually get the job done::

    sudo apt-get install -y python3-dev asound2-dev



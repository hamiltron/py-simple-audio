.. toctree::
   :maxdepth: 2

Simpleaudio Module Description
==============================

Overview
--------

The simplaudio module provides cross-platform audio playback
capability for Python 3 with a very simple interface. OSX, Windows, and Linux 
with ALSA are supported (see supported versions below for details). 
It is MIT Licensed.

Asynchronous
------------

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

Simpleaudio supports standard PCM signed integer formats - basically 
what is usually contained in a plain WAV file. 

Mono (1-channel) and Stereo (2-channel) audio is supported. 

The following sample rates (in Hz) are allowed, though not necessarily gauranteed 
to be supported on your platform/hardware. 

- 8000
- 11025
- 16000
- 22050
- 32000
- 44100
- 48000
- 88200
- 96000
- 192000

Installation
============

Install with:

pip3 install simpleaudio

Linux Dependencies
------------------

The Python 3 and ALSA development packages are required for pip to build
the extension. For Debian, Ubuntu, and Raspbian, this will usually get the job done: 

sudo apt-get install -y python3-dev asound2-dev



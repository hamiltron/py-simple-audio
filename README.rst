Simpleaudio
===========

Description
-----------

The simpleaudio module for Python 3 makes cross-platform audio playback
very simple. It plays audio from an existing, fixed-length buffer - i.e. 
it doesn't support real-time generated audio. It is designed to work 
seamlessly with the Wave module in the Python standard libarary. 

Platforms
---------

- Windows:
  - 7
  - 8/8.1 (not verified)
  - 10 (not verified)
  
- OSX:
  - 10.8 and below (not verified)
  - 10.9
  - 10.10 (not verified)

- Linux: should work on any disto with a successful build against ALSA (see installation notes below)

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

Installation should hopefully be as simple as 

::

pip3 install simpleaudio

Linux
-----

Unless someone creates a package for your distro, building from source is necessary. 
Luckily, pip automates the whole process. Only a few important packages (listed below) 
will need to be present before installing/building simpleaudio. One these are installed,
run the installation command above. 

LIST TODO

Simple Example
========

::

import simpleaudio as sa
wave_obj = sa.WaveObject.from_wave_file("my_wave.wav")
play_obj = wave_obj.play()
play_obj.wait_done()

Documentation
=============

TODO



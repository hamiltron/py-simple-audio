## To Do

* add docstrings to public classes/methods
* fix function check paragraph prints
* generate rest of audio format samples
* add latency options
* look into 24 and 32 bit audio
* add play ID to as many debug prints as possible
* add buffer pointers, etc to debug prints
* hook into module shutdown to explicitly stop audio playback
* look into possible handle/memrory leaks upon Win or ALSA errors
* investigate what happens when less than 1 (and 2) buffers-worth of audio is played
* check for memory allocation failure and return errors

## Tag-Release Cycle

1. update docs
2. update version strings
  1. docs/conf.py
  2. setup.py
3. run PEP8 check
4. build and run function checks on 3 platforms
5. update release notes
6. tag latest commit
7. build RTD docs
8. build distributions for Python 3.3, 3.4, 3.5
  * linux - sdist
  * osx - build and modify wheels to work on all versions
  * windows - build 32-bit
9. push to PyPI

## Windows Builds

[VS 2010 Link](http://download.microsoft.com/download/1/E/5/1E5F1C0A-0D5B-426A-A603-1798B951DDAE/VS2010Express1.iso)
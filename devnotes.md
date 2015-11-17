## To Do

* add docstrings to public classes/methods
* add latency options
* look into 24 and 32 bit audio
* add play ID to as many debug prints as possible
* add buffer pointers, etc to debug prints
* hook into module shutdown to explicitly stop audio playback
* look into possible handle/memrory leaks upon Win or ALSA errors
* check for memory allocation failure and return errors
* add external links (like pip & buffer protocol) to documentation
      
## Build Options

The `DEBUG` macro value (as set in *setup.py*) is used to turn on debug prints.
* `0` - Debug statements are disabled.
* `1` - Turns on debug statements that aren't in the playback inner-loop/thread/callback 
  (i.e. stuff that prints each time another chunk of audio is buffered). 
* `2` - Turns on all debug statements. This will spew continually while the audio is playing and may 
  cause dropouts due to time spent in print subroutines.  

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
  * linux (sdist)
  * osx 
  * windows
9. push to PyPI

## Windows Builds

[VS 2010 Link](http://download.microsoft.com/download/1/E/5/1E5F1C0A-0D5B-426A-A603-1798B951DDAE/VS2010Express1.iso)

[Super handy info on Windows builds](http://cowboyprogrammer.org/building-python-wheels-for-windows/)

Use VS 2015 Community edition for 3.5 (32 and 64 bit) builds. Select the C++ tools for installation. 

## Distribution Stuff

[amazing](https://tom-christie.github.io/articles/pypi/)

[package_data reference](https://docs.python.org/3.5/distutils/setupscript.html)

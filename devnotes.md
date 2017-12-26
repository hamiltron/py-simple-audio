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

## CI Notes

* Deploy is attempted only on tagged commits. Ideally it would also be
  only from the 'master' branch but AppVeyor doesn't support this so they're
  both setup to be consistent.
* Build from pull requests are disabled. For AppVeyor, this is done by
  excluding PR notifications from the webhook. For Travis, this is done in
  the project settings.  
* Twine creds: For Travis - set in project settings. For AppVeyor,
  encrypted and put in yml file

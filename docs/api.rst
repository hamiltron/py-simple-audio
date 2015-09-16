API
===

.. class:: PlayObject

.. function:: stop_all()

    Stop all currently playing audio.
   
.. function:: play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate):

    Start playback of audio data from an object supporting the buffer interface. 
    
    :param audio_data: object with audio data buffer
    :param int num_channels: the number of audio channels
    :param int bytes_per_sample: the number of bytes per single-channel sample
    :param int sample_rate: the sample rate in Hz 
    :rtype: a PlayObject for the audio being played
# Simpleaudio Python Extension
# Copyright (C) 2015, Joe Hamilton
# MIT License (see LICENSE.txt)

import _simpleaudio as _sa
from time import sleep

class PlayObject(object):
    def __init__(self, play_id):
        self.play_id = play_id

    def stop(self):
        _sa._stop(self.play_id)

    def wait_done(self):
        while self.is_playing():
            sleep(0.05)

    def is_playing(self):
        return _sa._is_playing(self.play_id)

def stop_all():
    _sa._stop_all()

def play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate):
    return _sa._play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate)

def play_wave_read(wave_read):
    n_chan = wave_read.getnchannels()
    samp_rate = wave_read.getframerate()
    samp_count = wave_read.getnframes()
    audio_data = wave_read.readframes(samp_count)
    return play_buffer(audio_data, wave_read.getnchannels(), wave_read.getsampwidth(), wave_read.getframerate())

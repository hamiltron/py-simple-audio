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
    play_id = _sa._play_buffer(audio_data, num_channels, bytes_per_sample, sample_rate)
    return PlayObject(play_id)

def play_wave_read(wave_read):
    wave_params = get_wave_params(wave_read)
    return play_buffer(*wave_params)

def get_wave_params(wave_read):
    return (wave_read.readframes(wave_read.getnframes()), wave_read.getnchannels(), wave_read.getsampwidth(), wave_read.getframerate())
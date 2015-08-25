# Simpleaudio Python Extension
# Copyright (C) 2015, Joe Hamilton 
# MIT License (see LICENSE.txt)

from _simpleaudio import play_buffer

def play_wave_read(wave_read):
    n_chan = wave_read.getnchannels()
    samp_rate = wave_read.getframerate()
    samp_count = wave_read.getnframes()
    #print("{} channels, {} Hz, {:.2f} seconds".format(n_chan, samp_rate, samp_count / samp_rate))
    audio_data = wave_read.readframes(samp_count)
    play_buffer(audio_data, wave_read.getnchannels(), wave_read.getsampwidth(), wave_read.getframerate())
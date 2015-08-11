from simpleaudio import *

import wave
import unittest
import os
import time

AUDIO_DIR = "test_audio"

class TestSimpleaudio(unittest.TestCase):
    
    def test_simple(self):
        with wave.open(os.path.join(AUDIO_DIR, "butts.wav"), 'rb') as wave_read:
            n_chan = wave_read.getnchannels()
            samp_rate = wave_read.getframerate()
            samp_count = wave_read.getnframes()
            print("{} channels, {} Hz, {:.2f} seconds".format(n_chan, samp_rate, samp_count / samp_rate))
            audio_data = wave_read.readframes(samp_count)
            play_buffer(audio_data, wave_read.getnchannels(), wave_read.getsampwidth(), wave_read.getframerate())
            time.sleep(4)

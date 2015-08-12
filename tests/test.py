import simpleaudio as sa

import wave
import unittest
import os
import time

AUDIO_DIR = "test_audio"

class TestSimpleaudio(unittest.TestCase):
    
    def test_simple(self):
        with wave.open(os.path.join(AUDIO_DIR, "butts.wav"), 'rb') as wave_read:
            sa.play_wave_read(wave_read)
            time.sleep(4)

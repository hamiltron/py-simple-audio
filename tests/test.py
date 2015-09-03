import simpleaudio as sa

import wave
import unittest
import os
from time import sleep

AUDIO_DIR = "test_audio"

class TestSimpleaudio(unittest.TestCase):
        
    def test_function_checks(self):
        sa.functionchecks.run_all()
        
    def test_num_channels(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 0, 2, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 3, 2, 44100)

    def test_bytes_per_chan(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 0, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 4, 44100)
        
    def test_sample_rate(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 2, 44101)

import simpleaudio as sa

import wave
import unittest
import os
import time

AUDIO_DIR = "test_audio"

class TestSimpleaudio(unittest.TestCase):
    def _test_file(self, file):
        with wave.open(os.path.join(AUDIO_DIR, file), 'rb') as wave_read:
            sa.play_wave_read(wave_read)
            time.sleep(4)

    def test_2_16_32(self):
        self._test_file("butts_2_16_32.wav")

    def test_2_16_44(self):
        self._test_file("butts_2_16_44.wav")

    def test_2_16_48(self):
        self._test_file("butts_2_16_48.wav")

    def test_num_channels(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 0, 2, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 3, 2, 44100)

    def test_bytes_per_chan(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 0, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 4, 44100)
        
    def test_sample_rate(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 2, 44101)

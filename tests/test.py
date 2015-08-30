import simpleaudio as sa

import wave
import unittest
import os
from time import sleep

AUDIO_DIR = "test_audio"

class TestSimpleaudio(unittest.TestCase):
    def _get_wave_params(self, file):
        with wave.open(os.path.join(AUDIO_DIR, file), 'rb') as wave_read:
            return sa.get_wave_params(wave_read)

    def test_2_16_32(self):
        with wave.open(os.path.join(AUDIO_DIR, "butts_2_16_32.wav"), 'rb') as wave_read:
            sa.play_wave_read(wave_read)
        sleep(4)
        
    def test_2_16_44(self):
        with wave.open(os.path.join(AUDIO_DIR, "butts_2_16_44.wav"), 'rb') as wave_read:
            sa.play_wave_read(wave_read)
        sleep(4)
        
    def test_2_16_48(self):
        with wave.open(os.path.join(AUDIO_DIR, "butts_2_16_48.wav"), 'rb') as wave_read:
            sa.play_wave_read(wave_read)
        sleep(4)
        
    def _test_overlap(self):
        wave_params = self._get_wave_params("butts_2_16_48.wav")
        sa.play_buffer(*wave_params)
        sleep(0.2)
        sa.play_buffer(*wave_params)
        sleep(0.2)
        sa.play_buffer(*wave_params)
        sleep(4)
        
    def _test_stop_first(self):
        wave_params = self._get_wave_params("butts_2_16_48.wav")
        play_1 = sa.play_buffer(*wave_params)
        sleep(0.1)
        play_2 = sa.play_buffer(*wave_params)
        sleep(1.0)
        print("#1 playing: ", play_1.is_playing())
        print("#2 playing: ", play_2.is_playing())
        play_1.stop()
        sleep(0.5)
        print("#1 playing: ", play_1.is_playing())
        print("#2 playing: ", play_2.is_playing())
        sleep(4)

    def _test_stop_second(self):
        wave_params = self._get_wave_params("butts_2_16_48.wav")
        play_1 = sa.play_buffer(*wave_params)
        sleep(0.1)
        play_2 = sa.play_buffer(*wave_params)
        sleep(1.0)
        print("#1 playing: ", play_1.is_playing())
        print("#2 playing: ", play_2.is_playing())
        play_2.stop()
        sleep(0.5)
        print("#1 playing: ", play_1.is_playing())
        print("#2 playing: ", play_2.is_playing())
        sleep(4)

    def _test_wait(self):
        wave_params = self._get_wave_params("butts_2_16_48.wav")
        play_1 = sa.play_buffer(*wave_params)
        play_1.wait_done()
        
    def _test_stop_all(self):
        wave_params = self._get_wave_params("butts_2_16_48.wav")
        sa.play_buffer(*wave_params)
        sleep(0.1)
        sa.play_buffer(*wave_params)
        sleep(0.1)
        sa.play_buffer(*wave_params)
        sleep(1)
        sa.stop_all()
        
    def test_num_channels(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 0, 2, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 3, 2, 44100)

    def test_bytes_per_chan(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 0, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 4, 44100)
        
    def test_sample_rate(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0'*16, 2, 2, 44101)

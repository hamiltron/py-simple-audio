import simpleaudio as sa

import unittest


class TestSimpleaudio(unittest.TestCase):

    def test_num_channels(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0' * 16, 0, 2, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0' * 16, 3, 2, 44100)

    def test_bytes_per_chan(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0' * 16, 2, 0, 44100)
        self.assertRaises(ValueError, sa.play_buffer, b'\0' * 16, 2, 5, 44100)

    def test_sample_rate(self):
        self.assertRaises(ValueError, sa.play_buffer, b'\0' * 16, 2, 2, 44101)

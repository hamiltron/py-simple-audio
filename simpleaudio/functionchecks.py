import simpleaudio as sa

import wave
import unittest
import os
from time import sleep

AUDIO_DIR = "test_audio"

def _gwp(wave_file):
    with wave.open(os.path.join(AUDIO_DIR, wave_file), 'rb') as wave_read:
        return sa.get_wave_params(wave_read)

def run_all(countdown=3):
    checks = [LeftRightCheck, OverlappingCheck, RatesAndChannelsCheck, 
              StopCheck, StopAllCheck, IsPlayingCheck]
    for check in checks:
        check.run(countdown)

class FunctionCheckBase(object):
    @classmethod
    def check(cls):
        raise NotImplementedError()
        
    @classmethod
    def run(cls, countdown=0):
        # print function check header
        print("")
        print("=" * 80)
        print("--", cls.__name__, "--")
        print(cls.__doc__.strip())
        print("=" * 80)
        
        if countdown > 0:
            print("Starting check in ...")
            for tick in reversed(range(1, countdown + 1)):
                print(tick, "...")
                sleep(1)
        print("RUNNING CHECK ...")
        cls.check()
        print("... DONE")
        

class LeftRightCheck(FunctionCheckBase):
    """
    This checks stereo playback by ...
    """
        
    @classmethod
    def check(cls):
        wave = _gwp("butts_2_16_44.wav")
        sa.play_buffer(*wave)
        sleep(4)
        
class OverlappingCheck(FunctionCheckBase):
    """
    This checks overlapped playback by ...
    """
        
    @classmethod
    def check(cls):
        wave_1 = _gwp("butts_2_16_44.wav")
        wave_2 = _gwp("butts_2_16_44.wav")
        wave_3 = _gwp("butts_2_16_44.wav")
        sa.play_buffer(*wave_1)
        sa.play_buffer(*wave_2)
        sa.play_buffer(*wave_3)
        sleep(4)
    
class RatesAndChannelsCheck(FunctionCheckBase):
    """
    This checks placback of mono and stereo audio at all allowed sample rates.
    """
    
    waves = [("butts_2_16_32.wav", 2, 32000),
             ("butts_2_16_44.wav", 2, 44100),
             ("butts_2_16_48.wav", 2, 48000)]
        
    @classmethod
    def check(cls):
        for wave_file, num_chan, sample_rate in cls.waves:
            wave = _gwp(wave_file)
            try:
                sa.play_buffer(*wave)
                sleep(4)
            except Exception as e:
                print("Error:", e.message)

class StopCheck(FunctionCheckBase):
    """
    This checks stopping playback by ...
    """
        
    @classmethod
    def check(cls):
        wave_1 = _gwp("butts_2_16_44.wav")
        wave_2 = _gwp("butts_2_16_44.wav")
        wave_3 = _gwp("butts_2_16_44.wav")
        play_1 = sa.play_buffer(*wave_1)
        play_2 = sa.play_buffer(*wave_2)
        play_3 = sa.play_buffer(*wave_3)
        sleep(0.5)
        play_1.stop()
        play_3.stop()
        sleep(3.5)
    
class StopAllCheck(FunctionCheckBase):
    """
    This checks stopping playback of all audio by ...
    """
        
    @classmethod
    def check(cls):
        wave_1 = _gwp("butts_2_16_44.wav")
        wave_2 = _gwp("butts_2_16_44.wav")
        wave_3 = _gwp("butts_2_16_44.wav")
        sa.play_buffer(*wave_1)
        sa.play_buffer(*wave_2)
        sa.play_buffer(*wave_3)
        sleep(0.5)
        sa.stop_all()
        sleep(3.5)

class IsPlayingCheck(FunctionCheckBase):
    """
    This checks stopping playback of all audio by ...
    """
        
    @classmethod
    def check(cls):
        wave = _gwp("butts_2_16_44.wav")
        play = sa.play_buffer(*wave)
        sleep(0.5)
        print("Is playing:", play.is_playing())
        sleep(4)
        print("Is playing:", play.is_playing())
import simpleaudio as sa

import os
from time import sleep

AUDIO_DIR = "test_audio"

def _gwo(wave_obj_file):
    return sa.WaveObject.from_wave_file(os.path.join(AUDIO_DIR, wave_obj_file))

def run_all(countdown=3):
    checks = [LeftRightCheck, OverlappingCheck, RatesAndChannelsCheck,
              StopCheck, StopAllCheck, IsPlayingCheck, WaitDoneCheck]
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
    This checks stereo playback by first playing a note
    in the left channel only, then a different note in the
    right channel only.
    """

    @classmethod
    def check(cls):
        wave_obj = _gwo("left_right.wav")
        wave_obj.play()
        sleep(4)

class OverlappingCheck(FunctionCheckBase):
    """
    This checks overlapped playback by playing three different notes
    spaced approximately a half-second apart but still overlapping.
    """

    @classmethod
    def check(cls):
        wave_obj_1 = _gwo("c.wav")
        wave_obj_2 = _gwo("e.wav")
        wave_obj_3 = _gwo("g.wav")
        wave_obj_1.play()
        sleep(0.5)
        wave_obj_2.play()
        sleep(0.5)
        wave_obj_3.play()
        sleep(3)

class RatesAndChannelsCheck(FunctionCheckBase):
    """
    This checks placback of mono and stereo audio at all allowed sample rates and bit-depths.
    """

    wave_files = ["notes_2_16_32.wav",
                  "notes_2_16_44.wav",
                  "notes_2_16_48.wav"]

    @classmethod
    def check(cls):
        for wave_file in cls.wave_files:
            wave_obj = _gwo(wave_file)
            try:
                wave_obj.play()
                sleep(4)
            except Exception as e:
                print("Error:", e.message)

class StopCheck(FunctionCheckBase):
    """
    This checks stopping playback by playing three different
    notes simultaneously and stopping two after approximately a half-second,
    leaving only one note playing for two more seconds.
    """

    @classmethod
    def check(cls):
        wave_obj_1 = _gwo("c.wav")
        wave_obj_2 = _gwo("e.wav")
        wave_obj_3 = _gwo("g.wav")
        play_obj_1 = wave_obj_1.play()
        wave_obj_2.play()
        play_obj_3 = wave_obj_3.play()
        sleep(0.5)
        play_obj_1.stop()
        play_obj_3.stop()
        sleep(3)

class StopAllCheck(FunctionCheckBase):
    """
    This checks stopping playback of all audio by playing three different
    notes simultaneously and stopping all of them after approximately
    a half-second.
    """

    @classmethod
    def check(cls):
        wave_obj_1 = _gwo("c.wav")
        wave_obj_2 = _gwo("e.wav")
        wave_obj_3 = _gwo("g.wav")
        wave_obj_1.play()
        wave_obj_2.play()
        wave_obj_3.play()
        sleep(0.5)
        sa.stop_all()
        sleep(3)

class IsPlayingCheck(FunctionCheckBase):
    """
    This checks functionality of the is_playing() method by
    calling during playback (when it should return True)
    and calling it again after all playback has stopped
    (when it should return False). The output is printed.
    """

    @classmethod
    def check(cls):
        wave_obj = _gwo("notes_2_16_44.wav")
        play_obj = wave_obj.play()
        sleep(0.5)
        print("Is playing:", play_obj.is_playing())
        sleep(4)
        print("Is playing:", play_obj.is_playing())

class WaitDoneCheck(FunctionCheckBase):
    """
    This checks functionality of the wait_done() method
    by using it to allow the three-note clip to play
    until finished (before attempting to stop playback).
    """

    @classmethod
    def check(cls):
        wave_obj = _gwo("notes_2_16_44.wav")
        play_obj = wave_obj.play()
        play_obj.wait_done()
        play_obj.stop()

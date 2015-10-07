import numpy as np
import simpleaudio as sa

# calculate note frequencies
A_freq = 440
E_freq = A_freq * 2 ** (7 / 12)
A2_freq = A_freq * 2

# get timesteps for each sample, T is note duration in seconds
sample_rate = 44100
T = 0.5 
t = np.linspace(0, T, T * sample_rate, False)

# generate sine wave notes
A_note = np.sin(A_freq * t * 2 * np.pi)
E_note = np.sin(E_freq * t * 2 * np.pi)
A2_note = np.sin(A2_freq * t * 2 * np.pi)

# concatenate notes
audio = np.hstack((A_note, E_note, A2_note))
# normalize to 16-bit range
audio *= 32767
# convert to 16-bit data 
audio = audio.astype(np.int16)

# start playback 
play_obj = sa.play_buffer(audio, 1, 2, sample_rate)

# wait for it to finish before exiting
play_obj.wait_done()
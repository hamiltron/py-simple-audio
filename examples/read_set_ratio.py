import simpleaudio as sa
import time

wave_obj = sa.WaveObject.from_wave_file("/Users/pmaal/repos/vocoder/data/youtube/The 30-Second Video_audio.wav")
play_obj = wave_obj.play()
while play_obj.is_playing():
	print(play_obj.read_ratio())
	time.sleep(1)
	print(play_obj.set_ratio(0.5))
	print("Right after setting to 0.5", play_obj.read_ratio())
	time.sleep(1)
	print("A second after setting to 0.5", play_obj.read_ratio())
	break
time.sleep(3)
play_obj.set_ratio(0.95)
time.sleep(1)
print(play_obj.read_ratio())
play_obj.stop()


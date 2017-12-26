# Simpleaudio Python Extension
# Copyright (C) 2015, Joe Hamilton
# MIT License (see LICENSE.txt)

from setuptools import setup, Extension
import sys
from os import path

platform_sources = []
platform_libs = []
platform_link_args = []
platform_compile_args = []

if sys.platform == 'darwin':
    platform_sources = ['c_src/simpleaudio_mac.c', 'c_src/posix_mutex.c']
    platform_link_args = ['-framework', 'AudioToolbox']
    platform_compile_args = ['-mmacosx-version-min=10.6']
elif sys.platform.startswith("linux"):
    platform_sources = ['c_src/simpleaudio_alsa.c', 'c_src/posix_mutex.c']
    platform_libs = ['asound']
elif sys.platform == 'win32':
    platform_sources = ['c_src/simpleaudio_win.c', 'c_src/windows_mutex.c']
    platform_libs = ['Winmm', 'User32']
else:
    pass
    # define a compiler macro for unsupported ?

simpleaudio_c_ext = Extension(
    'simpleaudio._simpleaudio',
    sources=platform_sources + ['c_src/simpleaudio.c'],
    libraries=platform_libs,
    extra_compile_args=platform_compile_args,
    extra_link_args=platform_link_args,
    define_macros=[('DEBUG', '0')])

# attempt to generate the version from git tag and commit

VERSION = "1.0.2"

# Get the long description from the relevant file
with open(path.join(path.abspath(path.dirname(__file__)), 'README.rst'),
          encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='simpleaudio',
    version=VERSION,
    license='MIT',
    description="Simple, asynchronous audio playback for Python 3.",
    long_description=long_description,
    author='Joe Hamilton',
    author_email='jhamilton10@georgefox.edu',
    url='https://github.com/hamiltron/py-simple-audio',
    keywords=['audio', 'wave', 'media', 'multimedia',
              'sound', 'alsa', 'coreaudio', 'winmm', 'music'],
    classifiers=['License :: OSI Approved :: MIT License',
                 'Programming Language :: Python :: 3.3',
                 'Programming Language :: Python :: 3.4',
                 'Programming Language :: Python :: 3.5',
                 'Programming Language :: Python :: 3.6',
                 'Topic :: Multimedia :: Sound/Audio',
                 'Operating System :: POSIX :: Linux',
                 'Operating System :: Microsoft :: Windows',
                 'Operating System :: MacOS :: MacOS X'],
    test_suite="tests",
    py_modules=["simpleaudio.shiny", "simpleaudio.functionchecks"],
    ext_modules=[simpleaudio_c_ext],
    packages=['simpleaudio'],
    package_dir={'simpleaudio': 'simpleaudio'},
    package_data={'simpleaudio': ['test_audio/c.wav', 'test_audio/e.wav',
                                  'test_audio/g.wav',
                                  'test_audio/left_right.wav',
                                  'test_audio/notes_2_16_44.wav']},)

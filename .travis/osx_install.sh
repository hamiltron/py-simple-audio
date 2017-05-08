#!/bin/bash

set -e
set -x

eval "$(pyenv init -)"
python setup.py bdist_wheel

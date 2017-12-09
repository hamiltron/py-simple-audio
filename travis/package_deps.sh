#!/bin/bash

set -e
set -x

which python3
which pip3
python3 -V
pip3 -V

pip3 install --upgrade pip
pip3 install --upgrade wheel setuptools twine

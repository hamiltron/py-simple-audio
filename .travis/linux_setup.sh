#!/bin/bash

set -e
set -x

which python
which pip
python -V
pip -V

pip install --upgrade pip
pip install --upgrade wheel setuptools twine pycodestyle

which pycodestyle

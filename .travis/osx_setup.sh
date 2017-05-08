#!/bin/bash

set -e
set -x

brew upgrade pyenv
pyenv install -s $PYENV_VERSION
eval "$(pyenv init)"

python -V
pip -V

pip install --upgrade pip
pip install --upgrade GitPython wheel setuptools 

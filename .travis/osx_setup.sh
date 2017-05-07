#!/bin/bash

set -e
set -x

brew update
brew upgrade pyenv
pyenv install -s $PYENV_VERSION

python -V
pip -V

pip install GitPython

#!/bin/bash

brew update
brew install pyenv
pyenv install -s $PYENV_VERSION
pip install GitPython

#!/bin/bash

set -e
set -x

python -V
pip -V

pip install --upgrade pip
pip install --upgrade GitPython wheel setuptools

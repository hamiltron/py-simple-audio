#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/${PY_VER}/python-${PY_VER}-macosx10.6.pkg -o pyinstall.pkg
sudo installer -package pyinstall.pkg -target /

which python3
which pip3
python3 -V
pip3 -V

pip3 install --upgrade pip
pip3 install --upgrade wheel setuptools

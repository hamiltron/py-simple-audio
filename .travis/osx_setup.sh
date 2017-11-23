#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/${PY_VER}/python-${PY_VER}-macosx10.6.pkg -o pyinstall.pkg
sudo installer -package pyinstall.pkg -target /


#python -V
#pip -V
#
#pip install --upgrade pip
#pip install --upgrade wheel setuptools

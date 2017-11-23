#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/3.3.5/python-3.3.5-macosx10.5.dmg -o pyinstall.dmg
hdiutil attach -mountpoint ./installmount pyinstall.dmg
sudo installer -package ./installmount/Python.mpkg -target /

which python3
which pip3
python3 -V
pip3 -V

pip3 install --upgrade pip
pip3 install --upgrade wheel setuptools

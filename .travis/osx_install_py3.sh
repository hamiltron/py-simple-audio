#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/${PY_VER}/python-${PY_VER}-macosx10.6.pkg -o pyinstall.pkg
sudo installer -package pyinstall.pkg -target /

# extra PATH addition for twine
export PATH=$PATH:$(dirname $(which python3))

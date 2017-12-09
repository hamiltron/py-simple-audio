#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/${PY_VER}/python-${PY_VER}-macosx10.6.pkg -o pyinstall.pkg
sudo installer -package pyinstall.pkg -target /

# extra PATH addition for twine
PY_VER_SHORT = $(echo $PY_VER | sed -E "s/([[:digit:]]+\.[[:digit:]]+)\.[[:digit:]]+/\1/")
export PATH=$PATH:/Library/Frameworks/Python.framework/Versions/$PY_VER_SHORT/bin

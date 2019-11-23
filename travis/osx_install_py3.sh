#!/bin/bash

set -e
set -x

curl https://www.python.org/ftp/python/${PY_VER}/${PY_PKG} -o pyinstall.pkg
sudo installer -package pyinstall.pkg -target /

# symlink for twine
PY_VER_SHORT=$(echo $PY_VER | sed -E "s/([[:digit:]]+\.[[:digit:]]+)\.[[:digit:]]+/\1/")
ln -s /Library/Frameworks/Python.framework/Versions/$PY_VER_SHORT/bin/twine /usr/local/bin/twine

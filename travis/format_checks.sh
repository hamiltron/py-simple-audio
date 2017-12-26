#!/bin/bash

pip3 install --upgrade wheel setuptools twine pycodestyle
which pycodestyle

ws_files=`find . | grep -E "\\.(py|c)$" | xargs -n 1 grep --files-with-matches -E "\\s$"`

if [ "$ws_files" != "" ]
then
    echo "-- TRAILING WHITESPACE --"
    echo "$ws_files"
    exit 1
fi

pep8_files=`find . | grep -E "\\.py$" | xargs -n 1 pycodestyle --ignore=E265`

if [ "$pep8_files" != "" ]
then
    echo "-- PEP 8 --"
    echo "$pep8_files"
    exit 1
fi

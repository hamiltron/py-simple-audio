#!/bin/bash

set -e
set -x

python3 setup.py build_ext --inplace 
python3 setup.py sdist

#!/bin/bash

set -e
set -x

twine upload -u hamiltron --repository-url https://test.pypi.org/legacy/ dist/*

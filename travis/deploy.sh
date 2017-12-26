#!/bin/bash

set -e
set -x

twine upload -u hamiltron dist/*

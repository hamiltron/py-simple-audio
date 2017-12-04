#!/bin/bash

set -e
set -x

twine upload --repository-url https://test.pypi.org/legacy/ dist/*

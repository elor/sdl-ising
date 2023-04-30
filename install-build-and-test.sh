#!/bin/bash
#
# This script installs the dependencies, builds the project and runs the tests.

set -e -u

mkdir -p build

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
(
cd build
ctest -V
)


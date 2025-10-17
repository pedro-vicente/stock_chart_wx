#!/bin/bash
set -e
# build directory 
mkdir -p build/trade
pushd build
pushd trade
cmake ../.. --fresh 
cmake --build . --config Debug --verbose
popd
popd

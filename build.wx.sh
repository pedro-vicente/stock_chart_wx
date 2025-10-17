#!/bin/bash
set -e
mkdir -p ext

if [ ! -d "ext/wxwidgets-3.3.1" ]; then
    echo "wxWidgets not found. Cloning from GitHub..."
    pushd ext
    git -c advice.detachedHead=false clone --branch v3.3.1 --depth 1 --progress https://github.com/wxWidgets/wxWidgets.git wxwidgets-3.3.1
    cd wxwidgets-3.3.1
    git submodule update --init --recursive --quiet
    popd
    echo "wxWidgets cloned successfully"
else
    echo "wxWidgets already exists in ext/wxwidgets-3.3.1"
fi

if [ ! -d "ext/asio-1.30.2" ]; then
    echo "ASIO not found. Cloning from GitHub..."
    pushd ext
    git -c advice.detachedHead=false clone --branch asio-1-30-2 --depth 1 --progress https://github.com/chriskohlhoff/asio.git asio-1.30.2
    popd
    echo "ASIO cloned successfully"
else
    echo "ASIO already exists in ext/asio-1.30.2"
fi

mkdir -p build/wxwidgets-3.3.1
# wxWidgets (-DwxBUILD_SAMPLES=SOME | ALL | OFF )
pushd build
pushd wxwidgets-3.3.1
cmake ../../ext/wxwidgets-3.3.1 -DwxBUILD_SHARED=OFF -DwxBUILD_SAMPLES=OFF
cmake --build .  --parallel 4
popd
popd
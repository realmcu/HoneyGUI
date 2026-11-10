#!/bin/bash
# Build libgui.a for WebAssembly. Run after "source <emsdk>/emsdk_env.sh",
# so that emcmake is in PATH.

if [ -d "build" ]; then
    rm -rf build
fi

if [ -d "install" ]; then
    rm -rf install
fi

emcmake cmake -S . -B build
cmake --build build -j$(nproc)
cmake --build build --target install

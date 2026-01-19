#!/bin/bash

if [ -d "build" ]; then
    rm -rf build
fi

if [ -d "install" ]; then
    rm -rf install
fi

mkdir -p build
cd build
cmake ..
make -j$(nproc)
make install
cd ..

read -p "是否拷贝到 honeygui-design 目录? (y/n): " choice
if [ "$choice" = "y" ] || [ "$choice" = "Y" ]; then
    cp install/lib/libgui.a /home/howie_wang/workspace/honeygui-design/lib/sim/linux/
    rm -rf /home/howie_wang/workspace/honeygui-design/lib/sim/include/*
    cp -r install/include/* /home/howie_wang/workspace/honeygui-design/lib/sim/include/
    echo "拷贝完成"
fi

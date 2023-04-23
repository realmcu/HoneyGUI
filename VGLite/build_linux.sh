#!/bin/bash
                                                                                               
usage()
{
    echo
    echo "Usage: $0  X86/ARM"
    echo
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

BOARD=$1
case "$BOARD" in

ARM)
    export SDK_DIR=./build
    export CROSS_COMPILE=/home/software/Linux/arm-vivante-linux-gnueabihf/bin/arm-vivante-linux-gnueabihf-
    export KERNEL_DIR=/home/software/Linux/linux-kernel
    export CPU_ARCH=armv7-a
    export ARCH=arm
    export ENABLE_PCIE=0
;;

X86)
    export SDK_DIR=./build
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export KERNEL_DIR=/home/software/Linux/x86_pcie/linux-headers-4.8.0-41-generic/
    export ENABLE_PCIE=1
    export CPU_ARCH=0
    export ARCH=x86
;;

*)
    echo
    echo "ERROR: Unknown [ $BOARD ], or not support so far."
    usage
;;
esac;

make clean
make install

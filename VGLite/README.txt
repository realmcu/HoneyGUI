
1. Build VGLite driver
======================

    a) Change KERNEL_DIR, CROSS_COMPILE and other build variables in ./build_linux.sh if necessary.

    b) The driver can be built directly on Linux X86 platform with a valid KERNEL_DIR setting.

        $ ./build_linux.sh install X86

    c) The driver can also be built with proper cross-compiler settings for ARM devices.

        $ ./build_linux.sh install ARM

    d) Clean build libraries and object files:

        $ ./build_linux.sh clean ARM


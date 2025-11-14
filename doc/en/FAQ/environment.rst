.. _FAQ_Environment_CN:

=======================
Development Environment
=======================

Simulator in VSCode
-------------------

If you encounter problems the first time you run the simulator in VSCode, please check the following configurations in your development environment:

Installing the Appropriate Version of the Toolchain
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Simulator in VSCode using MinGW toolchain (refer to `Install compiler <#Install_compiler_en>`_ in Get Started),
and MinGW version 8.1.0 is recommended, which can be accessed from `MinGW v8.1.0 Download <https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z>`_.
There is no guarantee that all later versions of the MinGW will function properly.

.. warning::
   VSCode currently does not support gdb version higher than v9.2.0 in MinGW. (gdb v8.1 is used in MinGW v8.1.0, which is recommended.)

Adding Toolchain to System Environment Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Make sure ``C:\\mingw64\\bin`` is already added to system environment variable ``Path``.
.. _FAQ_Environment_CN:

==========
开发环境
==========

在 VSCode 中使用模拟器
-----------------------

如果您首次在 VSCode 中运行模拟器时遇到问题，请检查开发环境中的以下配置：

安装适当版本的工具链
^^^^^^^^^^^^^^^^^^^^^

在 VSCode 中使用 MinGW 工具链进行模拟器开发，推荐使用 MinGW 版本 8.1.0，您可以从 `MinGW v8.1.0 Download <https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z>`_ 下载获取。不能保证所有更新版本的 MinGW 能够正常工作。

.. warning::

   VSCode 目前不支持使用高于 v9.2.0 的 MinGW 中的 gdb 版本。（推荐使用 MinGW v8.1.0 中的 gdb v8.1）

添加工具链到系统环境变量
^^^^^^^^^^^^^^^^^^^^^^^^^^

请确保 ``C:\\mingw64\\bin`` 已添加到系统环境变量 ``Path`` 中。
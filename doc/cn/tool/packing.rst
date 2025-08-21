.. _打包工具:

==================
打包工具
==================

RTL87x2G 和 RTL8762D
-----------------------
RTL87x2G 是一系列 IC 类型的缩写。

RTL87x2G 和 RTL8762D 的打包流程一样，以下以 RTL8762G 为例进行说明。

在开始之前，请在 SDK 目录下的（ ``\subsys\gui\realgui\example\screen_800_480\root_image_800_480`` ）选择一个合适的示例，或者基于示例创建一个新的打包目录。
然后，将 bat 和 py 脚本复制到该目录中，确保 root 文件夹以及 bat 和 py 脚本都处于该目录下。


用户数据生成流程如下：

.. figure:: https://foruda.gitee.com/images/1726730622033771482/882af9c3_13671125.png
   :align: center
   :width: 500px

1. 将所有生成的 ``bin`` 文件复制到文件夹 ``\src\app\dashboard\application\root_image\root`` 中。
2. 在 ``\src\app\dashboard\application\root_image`` 目录下，双击 ``mkromfs_0xa00000.bat`` 脚本文件以执行它，并生成 ``root`` 文件夹的镜像。执行后，该目录下将出现一个新的 ``.bin`` 文件和 ``.h`` 文件。
3. 其中， ``.h`` 文件包含了文件系统中每个文件的地址偏移量，可以在不使用文件系统的情况下直接访问。在开发 GUI 代码之前，请将包含 ``.h`` 的文件夹添加到包含目录中。
4. 其中， ``.bin`` 文件是根文件夹的镜像文件，使用 MPTool 工具将 ``root_xx.bin`` 文件烧录到Flash存储中。


RTL8763E 和 RTL8773DO
-----------------------

RTL8763E 是一个 IC 系列的名称，包含 RTL8763EWE-VP 和 RTL8763EW-VC。RTL8763E 和 RTL8773DO 打包流程类似。

在开始之前，请在 SDK 目录下的（ ``\tool\Gadgets\gui_package_tool`` ）选择对应 IC 的目录，8763EW 请选择 ``8763E`` 目录，8773DO 请选择 ``87x3D`` 目录。

生成用户数据过程如下：

.. figure:: https://foruda.gitee.com/images/1726730627101594191/d43820b6_13671125.png
   :align: center
   :width: 500px


1. 将所有生成的 bin 文件复制到文件夹 ``\tool\Gadgets\gui_package_tool\8763E\root`` 中。
2. 双击 ``\tool\Gadgets\gui_package_tool\8763E`` 目录中的 ``gen_root_image.bat`` 文件以执行脚本并生成 root 文件夹的镜像。一个新的 ``.bin`` 文件和 ``.h`` 文件将出现在目录中。
3. 其中， ``.h`` 文件包含了文件系统中每个文件的地址 偏移量，可以在不使用文件系统的情况下直接访问。
4. 其中， ``.bin`` 文件是根文件夹的镜像文件，使用 MPTool 工具将 ``root_xx.bin`` 文件烧录到 Flash 存储中。


RTL8773E
-----------------
RTL8773E 是一个 IC 系列的名称，包含 RTL8773EWE 和 RTL8773EWE-VP 等，用户数据打包处理过程如下：

生成用户数据
^^^^^^^^^^^^^^^^^

1. 将生成的图像 ``bin`` 文件复制到 ``\src\app\watch\gui_application\root_image\root\8773e_watch`` 目录，并将生成的字体 ``bin`` 文件复制到 ``\src\app\watch\gui_application\root_image\root\font`` 目录。
2. 修改构建地址（``build address``）：开发者需要通过修改 ``mkromfs_0x4400000.bat`` 文件（:kbd:`python_bin_mkromfs_0x4400000.py --binary --addr 0x238b400 root root(0x4400000).bin`）。这里的 ``--addr`` 参数对应于闪存映射中的 userdata 地址加上 0x400（图像头部大小）。
3. 在 ``\src\app\watch\gui_application\root_image`` 目录中双击 ``mkromfs_0x4400000.bat`` 文件来执行脚本，并生成 root 文件夹的镜像。一个新的 ``bin`` 文件 ``root(0x4400000).bin`` 和头文件 ``ui_resource.h`` 将出现在目录中。
4. 在这两个文件中， ``.bin`` 文件是包含 root 目录内容的镜像文件，而 ``.h`` 文件则包含了文件系统中每个文件的地址偏移量，这些偏移量允许开发者在不使用文件系统的情况下直接访问这些文件。

.. figure:: https://foruda.gitee.com/images/1726730908892819237/3349d8fb_13671125.png
   :align: center
   :width: 500px


.. note::
    生成的 ui_resource.h 文件需要手动添加以下代码。

.. code-block:: c
   
    #if defined _WIN32
    #else
    #include "flash_map.h"

    #define   MUSIC_NAME_BIN_ADDR       APP_DEFINED_SECTION_ADDR
    #define   MUSIC_HEADER_BIN_ADDR     (MUSIC_NAME_BIN_ADDR + 0xA000)
    #define   MUSIC_NAME_BIN_SIZE       (MUSIC_HEADER_BIN_ADDR - MUSIC_NAME_BIN_ADDR)
    #define   MUSIC_HEADER_BIN_SIZE     0x5000
    #endif


添加头部信息
^^^^^^^^^^^^^^^^^
使用 MPPG Tool 给用户数据文件添加头部信息，添加过程如下：

.. figure:: https://foruda.gitee.com/images/1726127049302320776/d8bc86b8_13671125.png
   :align: center
   :width: 700px

1. 选择菜单 :menuselection:`Tool-->Prepend header for user data`。
2. 添加 ``flash_map.ini`` 路径。
3. 添加用户数据文件（ ``root_xx.bin`` ）路径。
4. 生成可烧录的用户数据文件。


.. note::
    最大 size 必须大于实际 size，否则需要更改 flash_map 中的用户数据大小。

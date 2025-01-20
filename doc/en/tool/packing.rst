.. _Pack Tool:

==========
Pack Tool
==========

RTL87x2G and RTL8762D
----------------------

RTL87x2G is the abbreviation of a series IC type.

The packaging process for the RTL87x2G and RTL8762D is the same. Take RTL8762G as an example as follows.

Before starting, select the appropriate demo under the sdk directory (``\subsys\gui\realgui\example\screen_800_480\root_image_800_480``), or create a new packaging directory based on the example. 
Then copy the ``bat and py scripts`` to that directory, ensuring that the ``root folder`` and the ``bat and py scripts`` exist under the directory.

.. figure:: https://foruda.gitee.com/images/1726730622033771482/882af9c3_13671125.png
   :align: center
   :width: 500px

1. Copy ``resource`` all the generated bin files to the ``root`` folder.
2. Double-click the batch file ( ``.bat`` ) to run it, which will execute the packaging process and generate ``.bin`` file and ``.h`` file.
3. The ``.h`` is the address offset of each file in the file system, which can be accessed directly without using the file system. Before developing gui code, please add the folder containing ``.h`` to the include directory.
4. Use the MPTool tool to burn the ``.bin`` file into Flash memory.


RTL8763E and RTL8773DO
----------------------
RTL8763E is the name of a series IC type, including RTL8763EWE-VP/RTL8763EW-VC. The packaging process for RTL877DO is similar to that of RTL8763E.

Before starting, navigate to the SDK directory (``\tool\Gadgets\gui_package_tool``) and choose the appropriate IC directory. Select the ``8763E`` directory for ``RTL8763EW`` and the ``87x3D`` directory for ``RTL8773DO``.

The process for generating user data is as follows:

.. figure:: https://foruda.gitee.com/images/1726730627101594191/d43820b6_13671125.png
   :align: center
   :width: 500px

1. Copy all the generated bin files to the folder ``\tool\Gadgets\gui_package_tool\8763E\root``.
2. Double-click ``gen_root_image.bat`` in the ``\tool\Gadgets\gui_package_tool\8763E`` directory to execute the script and generate an image of the root folder. A new ``.bin`` file and ``.h`` file will appear in the directory.
3. Between them, ``.bin`` is the image file, and ``.h`` is the address offset of each file in the file system, which can be accessed directly without using the file system.


RTL8773E
-----------------
RTL8773E is the name of a series IC type, including RTL8773EWE/RTL8773EWE-VP. The user data packaging process is as follows:

Generate Root Bin
~~~~~~~~~~~~~~~~~~
1. Copy generated images bin to this folder ``\src\app\watch\gui_application\root_image\root\8773e_watch`` and Copy generated font bin to this folder ``\src\app\watch\gui_application\root_image\root\font``.
2. Modify build address: You need to adjust the address to ``0x238b400`` by modifying this file ``mkromfs_0x4400000.bat`` ( :kbd:`python_bin_mkromfs_0x4400000.py --binary --addr 0x238b400 root root(0x4400000).bin`). The ``--addr`` corresponds to the flash map userdata address +0x400 (image header size)
3. Double-click ``mkromfs_0x4400000.bat`` in the ``\src\app\watch\gui_application\root_image`` directory to execute the script and generate an image of the root folder. A new bin ``root(0x4400000).bin`` file and h file ``ui_resource.h`` will appear in the directory.
4. Between them, ``.bin`` is the image file, and ``.h`` is the address offset of each file in the file system, which can be accessed directly without using the file system.

.. figure:: https://foruda.gitee.com/images/1726730908892819237/3349d8fb_13671125.png
   :align: center
   :width: 500px


.. note::
    The generated ui_resource.h requires the following code to be added manually


.. code-block:: c
   
    #if defined _WIN32
    #else
    #include "flash_map.h"

    #define   MUSIC_NAME_BIN_ADDR       APP_DEFINED_SECTION_ADDR
    #define   MUSIC_HEADER_BIN_ADDR     (MUSIC_NAME_BIN_ADDR + 0xA000)
    #define   MUSIC_NAME_BIN_SIZE       (MUSIC_HEADER_BIN_ADDR - MUSIC_NAME_BIN_ADDR)
    #define   MUSIC_HEADER_BIN_SIZE     0x5000
    #endif


Adding Header Information
~~~~~~~~~~~~~~~~~~~~~~~~~
Using the MPPG Tool to add header information to user data files, the process is as follows:

.. figure:: https://foruda.gitee.com/images/1726127049302320776/d8bc86b8_13671125.png
   :align: center
   :width: 700px 

1. In the menu, select :menuselection:`Tool-->Prepend header for user data`.
2. Add the path to ``flash_map.ini``.
3. Add the path to the user data file (``root_xx.bin``).
4. Generate the burnable user data file.

.. note::
    The Max size must be larger than the Actual size; otherwise, the user data size in the flash_map needs to be changed.

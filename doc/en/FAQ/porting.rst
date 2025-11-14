.. _FAQ_Porting_EN:

=======
Porting
=======

User Data
---------

User Data bin image generation need to consider user data address in ``flash_map.h``. Normally the address
in generate script is consistent with user data address in ``flash_map.h``, and if user data bin need to
add image data header due to ``mppgtool`` requirement, the generate script address must increase by image
data header size.

JS Malloc Heap
--------------

JS (javascript) is included in term:`GUI` module, the heap space JS used may fail to malloc due to resource
limitation, so this heap space could relocate on term:`PSRAM` if SoC supports PSRAM feature. The specific
information can be found in the term:`API` ``void *context_alloc(size_t size, void *cb_data_p)``.

Feed Watch Dog
--------------

GUI task does not support feeding the watch dog, so the term:`APP` should do this in a hook function registered
by the APP and used by GUI. The registered function is ``void gui_task_ext_execution_sethook(void (*hook)(void))``.

Not Support FPU
---------------

If SoC does not support FPU, some headers and code should be excluded by macros, for example, RTL8763EP.

File System
-----------

SoC needs to read image and font resources from flash by file system which should set a start address
that is consistent with the address in User Data generation script. GUI has supplied the related file
which is ``romfs.c`` where the start address is ``ROMFS_ADDR``.

Flash Setting
-------------

Flash speed mode should be set to 4 bit mode; flash clock should be set to a higher value based on chip
capabilities.

CPU Frequence
-------------

CPU frequency should be set to a higher value based on chip capabilities.

SCONS Version
-------------

A specific scons version is required, please use the ``pip install scons==4.4.0`` command to download.

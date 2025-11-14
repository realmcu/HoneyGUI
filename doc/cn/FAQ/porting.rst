.. _FAQ_Porting_CN:

=======
移植
=======

用户数据
--------

生成用户数据 bin 镜像时，需要考虑 ``flash_map.h`` 中的用户数据地址。通常，生成脚本中的地址与 ``flash_map.h`` 中的用户数据地址一致。如果由于 mppgtool 要求，用户数据 bin 需要添加图像数据头，则生成脚本中的地址必须增加图像数据头大小。

JS 分配堆内存
--------------

JS ( JavaScript ) 包含在 term:`GUI` 模块中， JS 使用的堆空间可能会受到资源限制，导致 ``malloc`` 失败。如果 SoC 支持 term:`PSRAM` 特性，可以将此堆空间重定位到 PSRAM 。具体信息请参考 API ``void *context_alloc(size_t size, void *cb_data_p)``。

喂狗
----

GUI 任务不支持喂狗功能，因此应用程序应在 term:`APP` 注册的 hook 函数中进行喂狗操作，并由 GUI 使用。注册函数为 ``void gui_task_ext_execution_sethook(void (*hook)(void))``。

不支持 FPU
-----------

如果 SoC 不支持 FPU ，一些头文件和代码不应该包含在内，例如 RTL8763EP。

文件系统
--------

SoC 需要通过文件系统从 flash 中读取图像和字体资源，文件系统中的起始地址应与用户数据生成脚本中的地址一致。GUI 已经提供了相关文件，其中起始地址为 ``ROMFS_ADDR``。

Flash 设置
----------

Flash 速度模式应设置为 4 位模式；根据芯片的能力， Flash 时钟应设置为更高的值。

CPU 频率
--------

根据芯片的能力， CPU 频率应设置为更高的值。

SCONS 版本
----------

需要特定的 SCONS 版本，请使用 ``pip install scons==4.4.0`` 命令进行下载。

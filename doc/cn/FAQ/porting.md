# 移植

## 用户数据

生成用户数据bin镜像时，需要考虑flash_map.h中的用户数据地址。通常，生成脚本中的地址与flash_map.h中的用户数据地址一致。如果由于mppgtool要求，用户数据bin需要添加图像数据头，则生成脚本中的地址必须增加图像数据头大小。

## JS分配堆内存

JS(javascript)包含在GUI模块中，JS使用的堆空间可能会受到资源限制，导致malloc失败。如果SoC支持psram特性，可以将此堆空间重定位到psram。具体信息请参考api `void *context_alloc(size_t size, void *cb_data_p)`。

## 喂狗

GUI任务不支持喂狗功能，因此应用程序应在APP注册的hook函数中进行喂狗操作，并由GUI使用。注册函数为 `void gui_task_ext_execution_sethook(void (*hook)(void))`。

## 不支持FPU

如果SoC不支持FPU，一些头文件和代码不应该包含在内，例如RTL8763EP。

## 文件系统

SoC需要通过文件系统从flash中读取图像和字体资源，文件系统中的起始地址应与用户数据生成脚本中的地址一致。GUI已经提供了相关文件，其中起始地址为 `ROMFS_ADDR`。

## Flash设置

Flash速度模式应设置为4位模式；根据芯片的能力，Flash时钟应设置为更高的值。

## CPU频率

根据芯片的能力，CPU频率应设置为更高的值。

## SCONS版本

需要特定的SCONS版本，请使用 `pip install scons==4.4.0` 命令进行下载。

# 常见问题
## 开发环境常见问题
### 在VSCode中使用模拟器
如果您首次在VSCode中运行模拟器时遇到问题，请检查开发环境中的以下配置：
#### 安装适当版本的工具链
在VSCode中使用MinGW工具链进行模拟器开发（参见入门指南中的第3.1节），推荐使用MinGW版本8.1.0，您可以从 [MinGW v8.1.0 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)下载获取。不能保证所有更新版本的MinGW能够正常工作。
> 已知问题：VSCode目前不支持使用高于v9.2.0的MinGW中的gdb版本。（推荐使用MinGW v8.1.0中的gdb v8.1）

#### 添加工具链到系统环境变量
请确保 `C:/mingw64/bin` 已添加到系统环境变量 `Path`中。

## 移植常见问题
###  用户数据
生成用户数据bin镜像时，需要考虑flash_map.h中的用户数据地址。通常，生成脚本中的地址与flash_map.h中的用户数据地址一致。如果由于mppgtool要求，用户数据bin需要添加图像头，则生成脚本中的地址必须增加图像头大小。
###  JS分配堆内存
JS(javascript)包含在GUI模块中，JS使用的堆空间可能会受到资源限制，导致malloc失败。如果SoC支持psram特性，可以将此堆空间重定位到psram。具体信息请参考api `void *context_alloc(size_t size, void *cb_data_p)`。
###  喂狗
GUI任务不支持喂狗功能，因此应用程序应在APP注册的hook函数中进行喂狗操作，并由GUI使用。注册函数为 `void gui_task_ext_execution_sethook(void (*hook)(void))`.
###  创建Magic image
如果SoC不支持GPU，在创建Magic Image时，应该改用普通的图像创建api，具体信息请参考 `gui_magic_img_t *xml_gui_img_creat_from_mem(void *parent,  const char *name, void *addr,int16_t x, int16_t y)`.
###  不支持FPU
如果SoC不支持FPU，一些头文件和代码不应该包含在内，例如RTL8763EP。
###  文件系统
SoC需要通过文件系统从flash中读取图像和字体资源，文件系统中的起始地址应与用户数据生成脚本中的地址一致。GUI已经提供了相关文件，其中起始地址为 `ROMFS_ADDR`.
###  realloc函数
GUI需要此函数，但如果使用C库的realloc函数会导致hardfault，应用程序可以使用malloc函数代替realloc函数。
###  Flash设置
Flash速度模式应设置为4位模式；根据芯片的能力，Flash时钟应设置为更高的值。
###  CPU频率
根据芯片的能力，CPU频率应设置为更高的值。
###  SCONS版本
需要特定的SCONS版本，请使用 `pip install scons==4.4.0` 命令进行下载。
## 规格常见问题
###  图形
| 平台  | 8762D  |8772F   |87X2D |PC   |8763E|
|---|---|---|---|---|---|
| RGB565  |  是 | 是  | 是  | 是  | 是  |
|  RGB888 | 否  | 是  |  是 |  是 | 否  |
|  ARGB8888 |  否 | 是  |  是 | 是  | 否  |
|  SVG | 否  | 是   |  否 |  是  | 否  |
| TTF | 否  | 是   |  否 |  是 | 否  |
| DOT字体  |  是 | 是  | 是  | 是  |是  |
|  矢量图形 | 否  | 是   |  否 |  是  |否 |
|  线性渐变 | 否  | 是   |  否 |  是  |否 |
|  径向渐变 | 否  | 否   |  否 |  是  |否 |

###  内存使用量
#### 8772F示例

```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1qpLBVJRt2gnRR_cpuXc9L06VjqwvpzS0/preview" width="640" height="480" allow="autoplay"></iframe>
```
该示例的内存消耗统计如下：
| 模块 | 占用  |
|---|---|
| 控件  |  14.56KB |
| 帧缓冲区  |  屏幕宽度 * 像素字节数 * 行数 |
| 线程堆栈  |  10KB |

#### 控件内存使用量
| 控件 | 内存(字节)  |
|---|---|
|窗口 (win)|104|
|图像 (img)|248|
|开关 (switch)|384|
|文本 (text)|128|
|页面 (page)|128|
|标签视图 (tabview)|120|
|标签 (tab)|104|
|幕布视图 (curtainview)|152|
|幕布 (curtain)|104|
|按钮 (button)|408|
|拖动条 (seekbar)|416|

## 如何提高帧率常见问题

### 像素格式

使用RGBA/RGB图像可以获得出色的显示效果，但如果帧率较低，可以使用RGB565格式的图像资源，以稍微牺牲一些效果来提高帧率。

### 硬件加速

尽可能使用硬件加速来渲染图像，而不是软件加速。不同的芯片型号可能具有不同的GPU，请参考SDK中的指导文档获取详细信息。

### 数据传输速度

数据传输速度也会影响HoneyGUI的绘制速度，因此请确保内存的带宽和频率。

### UI设计

减少界面中的复杂性，以及单个界面中图像的数量和大小可以提高帧率。确保每个需要加载的图像数据的像素都是有用的。

### 图像压缩

几乎所有图像压缩都会降低UI的刷新率，因此如果内存大小足够，请避免使用压缩图像。

### 字体

#### 自定义二进制文件

* 尽可能使用字体大小为8的倍数。
* 当文件包含数百个字符时，在创建字体文件时应将indexMethod设置为0。

#### 标准TTF文件

* 使用TTF文件来显示文本比使用BMP文件要慢得多。
* TTF文件可以通过开源解决方案进行裁剪。

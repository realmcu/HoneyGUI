# 图像转换工具

## 图像格式转换
将各种格式的图片转换为RGB原始图片。
* 打开转换器。图片转换工具下载链接请参考该章节：[工具](../index.md) 

* 操作步骤及详细说明如下：

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718765954611644598/e761d204_13408154.png"></div>
<br/>

### 配置
- Color head : BeeGUI 渲染图像需要用到的头部信息。
- Big-endian : 输入图像是否采用大端模式。
- Compress : 启用图像压缩。
- MixAlphaChannel Flag : 在将RGBA转换为RGB或RGB565时，是否将alpha通道混合到RGB中。
- scanMode : 选择扫描方向是水平还是垂直，BeeGUI仅需要水平方向。
- Color Space : 选择颜色空间（RGB565、RGBA、BINARY...等）。

### 颜色空间

- RGB565: 色彩丰富但渲染成本和存储需求低，每个像素占2字节存储

|  Red |  Green |  Blue |
|----------|------------|-----------|
|  5bit|  6bit  |  5bit |

- ARGB8565: 24位的ARGB格式

|  Opacity |  Red |  Green |  Blue |
|----------|------------|-----------|--------------|
|  8bit|  5bit  |  6bit |    5bit  |

- RTKRGAB: 16位的RGAB格式

|  Red |  Green |  Opacity |  Blue |
|----------|------------|-----------|--------------|
|  5bit|  5bit  |  1bit |    5bit  |

- RGB : 24位RGB格式，每个像素占3个字节存储

|  Red |  Green |  Blue |
|----------|------------|-----------|
|  8bit|  8bit  |  8bit |

- ARGB : 带有透明度的真彩色，通过透明度效果提升显示质量。每个像素占4字节存储

|  Opacity |  Red |  Green |  Blue |
|----------|------------|-----------|--------------|
|  8bit|  8bit  |  8bit |    8bit  |

- BINARY : 使用一个位来表示一个像素

- RTKARGB8656 : 24位ARGB8565格式

|  Opacity |  Red |  Green |  Blue |
|----------|------------|-----------|--------------|
|  8bit|  5bit  |  6bit |    5bit  |


## 输出文件
转换后，将生成以下文件。
<br/>
<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718776957325643948/803a9ab3_13408154.png"></div>
<br/>

通过使用图像转换工具（ImageConvert Tool），开发者可以将三个JPG文件（a、b和c）转换为二进制文件（a、b和c）。

接下来，开发者需要将这些二进制文件放置在打包目录的根文件夹（root）中。关于打包资源的过程，请参考[打包工具](./packing.md)章节。

<div style="text-align: center"><img width= "300" src="https://foruda.gitee.com/images/1718786296198222012/e213af1a_13408154.png"></div>
<br/>


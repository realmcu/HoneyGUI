# 可缩放矢量图形

可缩放矢量图形（SVG）可以展示SVG格式的图像。
- SVG是一种用于描述二维图形的 XML 标记语言，与位图图像不同，SVG图像以文本形式存储，并且可以缩放到任意大小而不会失真，因为它们基于数学描述而不是像素。
- SVG图像可以直接打包到`root`文件夹。

## 用法
### 创建控件
使用[gui_svg_create_from_mem(parent, name, addr, size, x, y, w, h)](#gui_svg_create_from_mem)以从内存地址加载图像的方式创建一个SVG控件, 或者使用[gui_svg_create_from_file(parent, name, filename, x, y, w, h)](#gui_svg_create_from_file)以从文件系统路径加载图像的方式创建一个SVG控件。
其中，`w/h`是SVG控件的宽度和高度，而不是要绘制的SVG图像的宽高，`size`是必须填写的SVG图像的绘制数据大小。

### 设置旋转
使用[gui_svg_rotation(svg, degrees, c_x, c_y)](#gui_svg_rotation)将SVG图像旋转`degree`角度。其中，`(c_x, c_y)`是旋转中心。

### 设置缩放
如果需要更新SVG图像的大小，可以使用[gui_svg_scale(svg, scale_x, scale_y)](#gui_svg_scale)。

### 设置平移
使用[gui_svg_translate(svg, t_x, t_y)](#gui_svg_translate)平移SVG图像。

### 设置透明度
使用[gui_svg_set_opacity(svg, opacity_value)](#gui_svg_set_opacity)设置SVG图像透明度。

## 示例

### 创建一个简单的 SVG
```cpp
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
}
```
<br/>
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598969684310669/2e2a68e7_13671125.png"/></div>
<br/>

### SVG 旋转

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_rotation(svg1,90,50,50);
}
```
<br/>
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598974541085137/fcc74440_13671125.png"/></div>
<br/>

### SVG 缩放

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_scale(svg1,2,1);
}
```
<br/>
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598982107316449/af526d67_13671125.png"/></div>
<br/>

### SVG 平移

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_translate(svg1,100,100);
}
```
<br/>
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598986751661244/ff6ea9cf_13671125.png"/></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_svg.h

```

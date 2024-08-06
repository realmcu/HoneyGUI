# STB 图像
<br>

STB图像控件是用于显示图像的基本控件。可以设置左、中、右三种图像，中间的图像将会重复以匹配对象的宽度，STB图像控件支持 bmp、jpg、png、gif 等格式。

在 STB 图像库中，STB 并不是一个缩写，而是一个命名惯例，用于标识由 Sean T. Barrett 开发的一系列单文件库。STB 图像库`（stb_image.h）` 是其中之一，用于加载和存储多种常见的图像格式。

## 用法

### 创建控件

使用[gui_img_stb_create_from_mem(void *parent,  const char *name, void *addr, uint32_t size, uint8_t type, int16_t x, int16_t y);](#api) 从内存中创建一个STB图像控件，注意需要确保类型和大小是正确的。

### 设置属性

使用 [gui_img_stb_set_attribute(gui_stb_img_t *img, void *addr, uint32_t size, uint8_t type, int16_t x, int16_t y);](#api) 来设置STB图像控件的属性，可以设置新的文件和新的坐标。

## 示例

<details> <summary>code</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_obj.h"
#include "gui_app.h"
#include "gui_img.h"
#include "gui_img_stb.h"

static void app_home_ui_design(gui_app_t *app)
{
    gui_stb_img_t *jpg = gui_img_stb_create_from_mem(&app->screen, "jpg", TEST_JPG, 0x6640, JPEG, 0, 0);
    gui_stb_img_t *png = gui_img_stb_create_from_mem(&app->screen, "png", TEST_PNG, 0x2B00, PNG, 170, 170);
}
```

</details>

<br>

<center><img width= "370" src="https://foruda.gitee.com/images/1703146027234656357/48137b9c_9325830.png" /></center>
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_img_stb.h

```


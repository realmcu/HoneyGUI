# 简单图像

简单图像（simple img）是图像（img）控件的一个简化版本，去掉了旋转、平移和缩放等几何变换操作。

## 用法

### 创建控件

使用[gui_simple_img_create(void *parent,  const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h, IMG_SOURCE_MODE_TYPE src_mode_type)](#gui_simple_img_create)创建一个简单图像（simple img）控件。 其中，如果`src_mode_type`是`IMG_SRC_MEMADDR`，图像将从内存地址加载；如果`src_mode_type`是`IMG_SRC_FILESYS`，图像将从文件系统加载，`addr`的值应该是图片路径。加载方式请参考下列枚举：

```eval_rst
.. literalinclude:: ../../../gui_engine/engine/draw_img.h
   :language: c
   :start-after: /* IMG_SOURCE_MODE_TYPE enum start*/
   :end-before: /* IMG_SOURCE_MODE_TYPE enum end*/
```

### 设置位置

如果需要改变简单图像（simple img）控件的位置，使用 [gui_simple_img_set_location(gui_img_t *img, uint16_t x, uint16_t y)](#gui_simple_img_set_location)重设起点坐标。其中，`x, y` 是新的起点坐标。

### 设置属性

使用[void gui_simple_img_set_attribute(gui_simple_img_t *img, const char *name, void *addr,int16_t x,int16_t y)](#gui_simple_img_set_attribute)设置简单图像（simple img）控件的属性，改变图像来源并设置新的起点坐标。

### 获取高度/宽度

如果想获取简单图像（simple img）控件的高度或宽度，可以使用[gui_simple_img_get_height(gui_simple_img_t *this)](#gui_simple_img_get_height)或[gui_simple_img_get_width(gui_simple_img_t *this)](#gui_simple_img_get_width)。

## 示例

<details> <summary>代码片段</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_simple_img.h"
#include "gui_text.h"
#include "draw_font.h"

char *tb1_text = "gui_simple_img_create";

void page_tb1(void *parent)
{
    static char array1[50];
    static char array2[50];

    gui_font_mem_init(ARIALBD_SIZE16_BITS4_FONT_BIN);

    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);

    gui_text_t *text1 = gui_text_create(parent, "text1", 10, 100, 300, 30);
    gui_text_set(text1, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 130, 330, 30);
    gui_text_set(text2, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text2, LEFT);
    sprintf(array1, "gui_img_get_height %d", gui_simple_img_get_height(img_test));
    text2->content = array1;
    text2->len = strlen(array1);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 160, 330, 30);
    gui_text_set(text3, tb1_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(tb1_text), 16);
    gui_text_mode_set(text3, LEFT);
    sprintf(array2, "gui_img_get_width %d", gui_simple_img_get_width(img_test));
    text3->content = array2;
    text3->len = strlen(array2);
}

void page_tb2(void *parent)
{
    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);
    gui_simple_img_set_location(img_test, 50, 50);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 150, 330, 24);
    gui_text_set(text2, "gui_simple_img_set_location", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 27, 16);
    gui_text_mode_set(text2, LEFT);
}

void page_tb3(void *parent)
{
    gui_simple_img_t *img_test = gui_simple_img_create(parent, "simg", SET_ON_BIN, 0, 0, 0, 0, 0);
    gui_simple_img_set_attribute(img_test, "test", SET_OFF_BIN, 20, 20);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 120, 330, 24);
    gui_text_set(text3, "gui_simple_img_set_attribute", GUI_FONT_SRC_BMP, APP_COLOR_WHITE, 28, 16);
    gui_text_mode_set(text3, LEFT);
}

```

</details>

<br>

<center><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/widgets/simple_img.gif" /></center>
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_simple_img.h

```

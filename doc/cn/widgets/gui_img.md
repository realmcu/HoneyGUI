# 图像 (Img)
<br>

图像控件是用于显示图像的基本控件，支持移动、缩放、旋转等功能。

## 用法

### 创建控件

开发者可以使用 [gui_img_create_from_mem(void *parent,  const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_img_create_from_mem) 从内存中创建一个图像控件，或者使用 [gui_img_create_from_fs(void *parent, const char *file, int16_t x, int16_t y)](#gui_img_create_from_fs) 从文件系统中创建一个图像控件。同样，也可以使用 [gui_img_create_from_ftl(void *parent, const char *name, void *ftl, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_img_create_from_ftl) 从闪存中创建一个图像控件。
如果图像控件的宽度或高度设置为 0，那么控件的大小将根据图像源的大小自动设置。

### 更新位置

如果想要更新图像控件的位置，开发者可以使用 [gui_img_set_location(gui_img_t *_this, uint16_t x, uint16_t y)](#gui_img_set_location) 去更新位置。

### 设定属性

开发者可以通过 [gui_img_set_attribute(gui_img_t *_this, const char *name, void *addr, int16_t x, int16_t y)](#gui_img_set_attribute) 来设置图像控件的属性，替换为新图像并设置新坐标。

### 获取高度/宽度

如果想要获取图像控件的高度/宽度，开发者可以使用 [gui_img_get_height(gui_img_t *_this)](#gui_img_get_height) 或 [gui_img_get_width(gui_img_t *_this)](#gui_img_get_width)。

### 刷新大小

开发者可以调用 [gui_img_refresh_size(gui_img_t *_this)](#gui_img_refresh_size) 来刷新图像控件大小。

### 混合模式

开发者可以使用 [gui_img_set_mode(gui_img_t *_this, BLEND_MODE_TYPE mode)](#gui_img_set_mode) 来设定图像控件的混合模式。

### 移动

通过 [gui_img_translate(gui_img_t *_this, float t_x, float t_y)](#gui_img_translate) 来移动图像控件。
开发者可以将图像控件移动到新坐标，而不改变控件属性中的原始坐标。

### 旋转

开发者可以通过 [gui_img_rotation(gui_img_t *_this, float degrees, float c_x, float c_y)](#gui_img_rotation) 来围绕圆心旋转图像控件。

### 缩放

开发者可以使用 [gui_img_scale(gui_img_t *_this, float scale_x, float scale_y)](#gui_img_scale) 调整图像控件的大小以满足需求。

### 不透明度

图像控件的不透明度值是可调整的，开发者可以调用 [gui_img_set_opacity(gui_img_t *_this, unsigned char opacity_value)](#gui_img_set_opacity) 来调整。

### 动画效果

开发者可以通过 [gui_img_set_animate(gui_img_t *_this, uint32_t dur, int repeat_count, void *callback, void *p)](#gui_img_set_animate) 来设定图像控件的动画效果。

### 质量

开发者可以调用 [gui_img_set_quality(gui_img_t *_this, bool high_quality)](#gui_img_set_quality) 来设定图像控件的显示质量。

### 截屏

开发者可以使用 [gui_img_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf)](#gui_img_tree_convert_to_img) 来保存全屏截图。另外，可以使用 [gui_img_tree_convert_to_img_root_size(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf)](#gui_img_tree_convert_to_img_root_size) 来保存当前控件大小的截图。保存的图像会是RGB格式。

## 示例

<details> <summary>代码</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_text.h"
#include "draw_font.h"

char *tb1_text = "gui_img_create_from_mem";

void page_tb1(void *parent)
{
    static char array1[50];
    static char array2[50];

    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);

    gui_text_t *text1 = gui_text_create(parent, "text1", 10, 100, 300, 30);
    gui_text_set(text1, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 130, 330, 30);
    gui_text_set(text2, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
    gui_text_mode_set(text2, LEFT);
    sprintf(array1, "gui_img_get_height %d", gui_img_get_height(img_test));
    text2->utf_8 = array1;
    text2->len = strlen(array1);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 160, 330, 30);
    gui_text_set(text3, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
    gui_text_mode_set(text3, LEFT);
    sprintf(array2, "gui_img_get_width %d", gui_img_get_width(img_test));
    text3->utf_8 = array2;
    text3->len = strlen(array2);
}

void page_tb2(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
    gui_img_set_location(img_test, 50, 50);

    gui_text_t *text2 = gui_text_create(parent, "text2", 10, 100, 330, 24);
    gui_text_set(text2, "gui_img_set_location", GUI_FONT_SRC_BMP, 0xffffffff, 20, 24);
    gui_text_mode_set(text2, LEFT);
}

void page_tb3(void *parent)
{
    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
    gui_img_set_attribute(img_test, "test", SET_OFF_BIN, 20, 20);

    gui_text_t *text3 = gui_text_create(parent, "text3", 10, 100, 330, 24);
    gui_text_set(text3, "gui_img_set_attribute", GUI_FONT_SRC_BMP, 0xffffffff, 21, 24);
    gui_text_mode_set(text3, LEFT);

}

void page_tb4(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
    gui_img_scale(img_test, 0.5, 0.5);

    gui_text_t *text4 = gui_text_create(parent, "text4", 10, 100, 330, 24);
    gui_text_set(text4, "gui_img_scale", GUI_FONT_SRC_BMP, 0xffffffff, 13, 24);
    gui_text_mode_set(text4, LEFT);
}

void page_tb5(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
    gui_img_translate(img_test, 100, 100);

    gui_text_t *text5 = gui_text_create(parent, "text5", 10, 100, 330, 24);
    gui_text_set(text5, "gui_img_translate", GUI_FONT_SRC_BMP, 0xffffffff, 17, 24);
    gui_text_mode_set(text5, LEFT);
}

void page_tb6(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
    gui_img_rotation(img_test, 10, 0, 0);

    gui_text_t *text6 = gui_text_create(parent, "text6", 10, 100, 330, 24);
    gui_text_set(text6, "gui_img_rotation", GUI_FONT_SRC_BMP, 0xffffffff, 16, 24);
    gui_text_mode_set(text6, LEFT);
}
```

</details>

<br>

<center><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/widgets/img.gif" /></center>
<br>


## API


```eval_rst

.. doxygenfile:: gui_img.h

```


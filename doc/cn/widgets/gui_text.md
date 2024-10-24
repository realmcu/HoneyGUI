# 文本 (Text)

文本控件是用于显示文本的基本控件，可用于将不同字体、不同颜色和不同大小的文本输出到屏幕上。为了绘制文本，字体文件可以是标准的.ttf文件或自定义的.bin文件。

## 特性

文本控件能支持下面的特性。

+ **支持UTF-8/UTF16/UTF-32**
+ **支持多语言**
+ **支持文本排版**
+ **自动换行和文本滚动**
+ **抗锯齿**
+ **支持多种字体**
+ **支持多种字体大小**
+ **支持32位真彩色**
+ **支持自定义动画效果**
+ **支持EMOJI表情**
+ **支持标准TTF格式字体[^1]**
+ **支持自行开发的字体文件**

[^1]: 只有部分芯片支持此功能

## 用法

调用对应的函数加载字体文件并显示文本。

### 初始化字体文件

为了绘制文本，包含字体信息的字体文件需要加载到系统中。

字体文件可以是标准的.ttf文件或自定义的.bin文件。字体文件需要提前初始化，且必须为文本控件设置字体类型。

+ 初始化自定义bin字体文件，需要调用 `gui_font_mem_init(uint8_t *font_bin_addr)`。

+ 初始化标准TTF文件来绘制文本，需要调用 `gui_font_stb_init(void *font_ttf_addr)`。

所有自定义bin字体文件都可以从RTK FAE那里获得。

`FONT_BIN`, `FONT_TTF` 这两个文件储存了flash中的文件地址。

### 创建文本控件

开发者可以调用 [gui_text_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_text_create) 来创建文本控件。创建后，控件的坐标和文本框的大小已经确定。这些属性也可以随时修改。

```{note}
文本控件的大小应大于要显示的字符串，超出范围的文本将被隐藏。
```

### 设置文本属性

#### 设置文本

开发者可以调用 [gui_text_set(gui_text_t *this_widget, void *text, FONT_SRC_TYPE text_type, gui_color_t color, uint16_t length, uint8_t font_size)](#gui_text_set) 来设置文本控件文本、文本类型、颜色、长度和文本字体大小。

```{note}
文本长度必须与设置的字符长度相同，文本字体大小必须与加载的字体文件的大小相同。
```

#### 字体类型

文本控件支持类型设置。开发者可以调用 [gui_text_type_set(gui_text_t *this_widget, void *font_source, FONT_SRC_MODE font_mode)](#gui_text_type_set) 来设置类型。类型为bin/ttf文件的地址。

#### 文本内容

开发者可以调用 [gui_text_content_set(gui_text_t *this_widget, void *text, uint16_t length)](#gui_text_content_set) 来设置文本控件需要显示的内容。

#### 文本编码

文本控件同时支持UTF-8编码、UTF-16编码和UTF32编码输入格式，开发者可以使用 [gui_text_encoding_set(gui_text_t *this_widget, TEXT_CHARSET charset)](#gui_text_encoding_set) 更改编码方式。

#### 文本转换为图片

使用 [gui_text_convert_to_img(gui_text_t *this_widget, GUI_FormatType font_img_type)](#gui_text_convert_to_img) 可以将文本控件中的文本将被转换为图像，存储在内存中，并使用该图像进行呈现。它还支持图像转换，如缩放和旋转。这只适用于位图字体。

```{note}
因为需要文本控件的内容和字体大小信息，所以应该在set text之后调用它。如果修改了文本的内容、字体大小、位置和其他属性，则需要重用此接口进行转换。
```

#### 文本输入设置

开发者可以使用函数 [gui_text_input_set(gui_text_t *this_widget, bool inputable)](#gui_text_input_set) 设置文本的输入。

#### 设置文本点击事件

开发者可以调用函数 [gui_text_click(gui_text_t *this_widget, gui_event_cb_t event_cb, void *parameter)](#gui_text_click) 添加文本点击事件。

#### 文本模式

文本控件支持七种排版模式，通过 [gui_text_mode_set(gui_text_t *this_widget, TEXT_MODE mode)](#gui_text_mode_set) 来设置文本排版模式。

排版模式如下：

| 类型                | 行类型         | X 方向             | Y 方向             | 控件                   |
|---------------------|----------------|-------------------|--------------------|------------------------|
| `LEFT`              | 单行           | 左对齐             | 顶部               | 文本控件(默认)         |
| `CENTER`            | 单行           | 居中               | 顶部               | 文本控件               |
| `RIGHT`             | 单行           | 右对齐             | 顶部               | 文本控件               |
| `MULTI_LEFT`        | 多行           | 左对齐             | 顶部               | 文本控件               |
| `MULTI_CENTER`      | 多行           | 居中               | 顶部               | 文本控件               |
| `MULTI_RIGHT`       | 多行           | 右对齐             | 顶部               | 文本控件               |
| `MID_LEFT`          | 多行           | 左对齐             | 中部               | 文本控件               |
| `MID_CENTER`        | 多行           | 居中               | 中部               | 文本控件               |
| `MID_RIGHT`         | 多行           | 右对齐             | 中部               | 文本控件               |
| `SCROLL_X`          | 单行           | 由右向左滚动       | 顶部               | 滚动文本控件           |
| `SCROLL_Y`          | 多行           | 左对齐             | 由下向上滚动       | 滚动文本控件           |
| `SCROLL_Y_REVERSE`  | 多行           | 右对齐             | 由上向下滚动       | 滚动文本控件           |
| `VERTICAL_LEFT`     | 多行           | 左对齐             | 由上向下           | 文本控件               |
| `VERTICAL_RIGHT`    | 多行           | 右对齐             | 由下向上           | 文本控件               |

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_text.h
   :language: c
   :start-after: /** @brief  text mode enum start */
   :end-before: /** @brief  text mode enum end */
```

#### 文本移动

开发者可以调用函数 [gui_text_move(gui_text_t *this_widget, int16_t x, int16_t y)](#gui_text_move) 将文本移动到指定位置，但是x和y不能大于文本控件的w和h。

#### 设置动画

开发者可以调用函数 [gui_text_set_animate(void *o, uint32_t dur, int repeat_count, void *callback, void *p)](#gui_text_set_animate) 来设置动画并在相应的回调函数中实现动画效果。

## 示例

### 多文本控件

<details> <summary>代码</summary>

```C
#include "string.h"
#include "gui_obj.h"
#include "guidef.h"
#include "gui_text.h"
#include "draw_font.h"
#include "gui_app.h"
#include "rtk_gui_resource.h"

static char chinese[6] =
{
    0xE4, 0xB8, 0xAD,
    0xE6, 0x96, 0x87
};
static void app_launcher_ui_design(gui_app_t *app)
{
    gui_font_mem_init(HARMONYOS_SIZE24_BITS1_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE16_BITS4_FONT_BIN);
    gui_font_mem_init(HARMONYOS_SIZE32_BITS1_FONT_BIN);
    gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);

    void *screen = &app->screen;

    gui_text_t *text1 = gui_text_create(screen,  "text1",  10, 10, 100, 50);
    gui_text_set(text1, chinese, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(chinese), 24);
    gui_text_type_set(text1, HARMONYOS_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(screen,  "text2",  0, 50, 300, 50);
    gui_text_set(text2, "english", GUI_FONT_SRC_BMP, APP_COLOR_RED, 7, 16);
    gui_text_type_set(text2, HARMONYOS_SIZE16_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text2, LEFT);

    char *string = "TEXT_WIDGET";
    gui_text_t *text3 = gui_text_create(screen,  "text3",  0, 90, 300, 50);
    gui_text_set(text3, string, GUI_FONT_SRC_BMP, APP_COLOR_BLUE, strlen(string), 32);
    gui_text_type_set(text3, HARMONYOS_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text3, CENTER);

    gui_text_t *text4 = gui_text_create(screen,  "text4",  0, 150, 100, 200);
    gui_text_set(text4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", GUI_FONT_SRC_BMP, gui_rgb(0, 0xff, 0xff), 24, 24);
    gui_text_type_set(text4, SIMKAI_SIZE24_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text4, MULTI_CENTER);
}

```

</details></br>

<center><img  width="300" src= "https://foruda.gitee.com/images/1694429576419596614/3cc7bc43_9325830.png "/></center></br>

### 动画文本控件


<details> <summary>代码</summary>

```C
#include "root_image_hongkong/ui_resource.h"
#include "string.h"
#include "gui_obj.h"
#include "guidef.h"
#include "gui_text.h"
#include "draw_font.h"

void change_text_cb(gui_text_t *obj)
{
    if (obj->animate->current_frame > 0 && obj->animate->current_frame < 50)
    {
        gui_text_move(obj, 50, 150);
        gui_text_content_set(obj, "123456789", 9);
    }
    else if (obj->animate->current_frame > 50 && obj->animate->current_frame < 100)
    {
        gui_text_move(obj, 200, 150);
        gui_text_content_set(obj, "987654321", 9);

    }
    else
    {
        gui_text_move(obj, 125, 50);
        gui_text_content_set(obj, "abcdefghi", 9);
    }
}

void page_tb_activity(void *parent)
{
    gui_font_mem_init(SIMKAI_SIZE24_BITS4_FONT_BIN);

    gui_text_t *text = gui_text_create(parent,  "text",  0, 0, 100, 200);
    gui_text_set(text, "ABCDEFGHI", GUI_FONT_SRC_BMP, APP_COLOR_RED, 9, 24);
    gui_text_type_set(text, SIMKAI_SIZE24_BITS4_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_CENTER);
    gui_text_set_animate(text, 5000, 15, change_text_cb, text);
}

```

</details></br>

<center><img width="300" src= "https://docs.realmcu.com/HoneyGUI/image/widgets/text.gif"/></center>

<br>

## API

```eval_rst

.. doxygenfile:: gui_text.h

```

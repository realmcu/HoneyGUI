# 进度条控件
<br>

进度条控件允许创建具备三种不同效果的控件。开发者创建进度条控件后，可以设置控件显示的百分比。

## 用法

### 创建水平方向控件

开发者可以使用API [gui_progressbar_img_h_create(void *parent, void *picture, int16_t x, int16_t y)](#gui_progressbar_img_h_create)创建基于单张图片的水平方向进度条控件。 这是一个普通的单张图片进度条控件。

### 创建垂直方向控件

类似于上面提到的`创建水平方向控件`，开发者可以使用 [gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y)](#gui_progressbar_img_v_create) 创建基于单张图片的垂直方向进度条控件。

### 创建电影风格控件

与上述两种单张图片的进度条控件不同，电影风格的进度条控件由一系列的图片组成。开发者可以调用[gui_progressbar_movie_create(void *parent, void  **picture_array, uint16_t array_length, int16_t x, int16_t y)](#gui_progressbar_movie_create)从内存地址创建电影风格的进度条控件，或者调用 [gui_progressbar_movie_create_from_fs(void *parent, void  **picture_array, uint16_t array_length, int16_t x, int16_t y)](#gui_progressbar_movie_create_from_fs) 从文件系统创建电影风格的进度条控件。
在 0% 进度时显示第一张图片，在 100% 进度时显示最后一张图片。

### 进度

例如，开发者想创建一个电影风格的进度条控件，可以通过 [gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress)](#gui_progressbar_set_progress) 和 [gui_progressbar_get_progress(gui_progressbar_t *this)](#gui_progressbar_get_progress) 来设置和获取进度。

### 百分比

上述用法(`进度`)适用于电影风格的进度条控件。对于普通的单张图片进度条控件，如果开发者想使用一个进度条控件来显示歌曲播放的进度，可以在创建进度条控件后使用 [gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage)](#gui_progressbar_set_percentage) 来设置当前的百分比进度。此外，还可以使用 [gui_progressbar_get_percentage(gui_progressbar_t *this)](#gui_progressbar_get_percentage) 来获取当前的百分比进度。

### 获取最大值

此 API [gui_progressbar_get_max(gui_progressbar_t *this)](#gui_progressbar_get_max) 用于获取电影风格进度条控件中的图片数量，这样就可以知道总共会显示多少张图片。

## 示例

<details> <summary>示例代码</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include "gui_progressbar.h"
#include "gui_text.h"
#include "draw_font.h"

static void *array[] = {DOG40_BIN, DOG60_BIN, DOG80_BIN, DOG100_BIN, DOG120_BIN, DOG140_BIN};

void page_tb_one(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_progressbar_t *test_bar_one = gui_progressbar_img_v_create(parent, TEST_BIN, 50, 0);
    gui_progressbar_set_percentage(test_bar_one, 1);

    gui_progressbar_t *test_bar_two = gui_progressbar_img_v_create(parent, TEST_BIN, 150, 0);
    gui_progressbar_set_percentage(test_bar_two, 0.5);

    gui_text_t *text1 = gui_text_create(parent, "vertical1", 50, 250, 300, 24);
    gui_text_set(text1, "100%    50%  vertical", GUI_FONT_SRC_BMP, 0xffffffff, 21, 24);
    gui_text_mode_set(text1, LEFT);

    gui_text_t *text2 = gui_text_create(parent, "vertical2", 10, 300, 330, 24);
    gui_text_set(text2, "gui_progressbar_img_v_create", GUI_FONT_SRC_BMP, 0xff0000ff, 28, 24);
    gui_text_mode_set(text2, LEFT);
}

void page_tb_two(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_progressbar_t *test_bar_three = gui_progressbar_img_h_create(parent, TEST_BIN, 50, 0);
    gui_progressbar_set_percentage(test_bar_three, 1);

    gui_progressbar_t *test_bar_four = gui_progressbar_img_h_create(parent, TEST_BIN, 150, 0);
    gui_progressbar_set_percentage(test_bar_four, 0.5);

    gui_text_t *text3 = gui_text_create(parent, "horizontal1", 50, 250, 300, 24);
    gui_text_set(text3, "100%    50%  horizontal", GUI_FONT_SRC_BMP, 0xffffffff, 23, 24);
    gui_text_mode_set(text3, LEFT);

    gui_text_t *text4 = gui_text_create(parent, "horizontal2", 10, 300, 330, 24);
    gui_text_set(text4, "gui_progressbar_img_h_create", GUI_FONT_SRC_BMP, 0xff0000ff, 28, 24);
    gui_text_mode_set(text4, LEFT);
}

void page_tb_three(void *parent)
{
    gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

    gui_progressbar_t *test_bar_five = gui_progressbar_movie_create(parent, array, 6, 0, 0);

    gui_progressbar_t *test_bar_six = gui_progressbar_movie_create(parent, array, 6, 150, 0);
    gui_progressbar_set_progress(test_bar_six, 4);

    gui_text_t *text5 = gui_text_create(parent, "movie1", 80, 250, 300, 24);
    gui_text_set(text5, "pic0   pic4  movie", GUI_FONT_SRC_BMP, 0xffffffff, 18, 24);
    gui_text_mode_set(text5, LEFT);

    gui_text_t *text6 = gui_text_create(parent, "movie2", 10, 300, 330, 24);
    gui_text_set(text6, "gui_progressbar_movie_create", GUI_FONT_SRC_BMP, 0xff0000ff, 28, 24);
    gui_text_mode_set(text6, LEFT);
}
```

</details><br>

<center><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/widgets/progressbar.gif" /></center>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_progressbar.h

```


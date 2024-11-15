# Progressbar

The progressbar widget allows the creation of widgets with three different effects.
After creating a progressbar widget, the percentage of the widget to display can be set.

## Usage

### Create Horizontal Widget

A horizontal style progressbar based on a picture can be created by this API [gui_progressbar_img_h_create(void *parent, void *picture, int16_t x, int16_t y)](#gui_progressbar_img_h_create).
It's a normal single-picture progressbar.

### Create Vertical Widget

Similar to `Create Horizontal Widget` above, [gui_progressbar_img_v_create(void *parent, void *picture, int16_t x, int16_t y)](#gui_progressbar_img_v_create) can be used to create a vertical style progressbar based on a picture.

### Create Movie-Style Widget

Unlike the above two normal single-picture progressbars, a movie-style progressbar consists of a series of pictures.
A movie style progressbar can be created from a memory address by using [gui_progressbar_movie_create(void *parent, void  **picture_array, uint16_t array_length, int16_t x, int16_t y)](#gui_progressbar_movie_create) , and [gui_progressbar_movie_create_from_fs(void *parent, void  **picture_array, uint16_t array_length, int16_t x, int16_t y)](#gui_progressbar_movie_create_from_fs) from a filesystem.
The first picture is shown at 0% progress and the last picture is shown at 100% progress.

### Progress

For example, if creating a movie-style progress bar is desired, [gui_progressbar_set_progress(gui_progressbar_t *this, size_t progress)](#gui_progressbar_set_progress) can be used to set the progress, and [gui_progressbar_get_progress(gui_progressbar_t *this)](#gui_progressbar_get_progress) can be used to get the progress.

### Percentage

The above usage of (`Progress`) applies to a movie-style progress bar. For a normal single-image progress bar, if showing the progress of a song with a progress bar is desired, [gui_progressbar_set_percentage(gui_progressbar_t *this, float percentage)](#gui_progressbar_set_percentage) can be used to set the current percentage progress after creating the progress bar. Additionally, [gui_progressbar_get_percentage(gui_progressbar_t *this)](#gui_progressbar_get_percentage) can be used to get the current percentage progress.

### Get Max Number

This API [gui_progressbar_get_max(gui_progressbar_t *this)](#gui_progressbar_get_max) is used to get the number of pictures in a movie-style progress bar, so the total number of pictures that will be displayed can be known.

## Example

<details> <summary>code</summary>

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


</details>

<br>

<center><img width= "400" src="https://docs.realmcu.com/HoneyGUI/image/widgets/progressbar.gif" /></center>

## API

```eval_rst

.. doxygenfile:: gui_progressbar.h

```

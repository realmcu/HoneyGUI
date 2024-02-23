# Cube
<br/>

## Overview
The cube is a 3D display widget that show a rotating cube with six faces, each capable of displaying distinct images.

## Usage
### Create a cube
[gui_cube_t *gui_cube_create(void *parent,  const char *name, gui_cube_imgfile_t *img_file, int16_t x, int16_t y)](#api) creates a cube widget. The `img_file` is an struct including image sources for the six faces of a cube. Both memory address and file path are supported, whether you choose to utilize memory or access the filesystem, simply set `IMG_SOURCE_MODE_TYPE src_mode[6]` to either `IMG_SRC_MEMADDR` or `IMG_SRC_FILESYS`.

### Set size
By default, the cube's size is `dc->fb_height / 8.0`. If you want to change the size set [void gui_cube_set_size(cube, size)](#api). Note this `size` is the 1/2 length of cube edge.

### Set center
By default, the center is `((dc->fb_width - size) / 2.0f,dc->fb_width - size) / 2.0f)`. You can use [void gui_cube_set_center(cube, c_x, c_y)](#api) to set the center coordinates of the cube.

### Set image mode
By default, the cube's image blend mode is `IMG_FILTER_BLACK`, you can change the blend mode of image by calling [void gui_cube_set_mode(gui_cube_t *cube, CUBE_SIDE_TYPE cube_side, BLEND_MODE_TYPE mode)](#api).

### Set cube image
The images of cube can be configured by calling [void gui_cube_set_img(gui_cube_t *cube, gui_cube_imgfile_t *img_file)](#api).

### Set rotation direction
The rotation direction of this cube can be achieved by calling [void gui_cube_auto_rotation_by_x(gui_cube_t *this, uint32_t internal_ms, float degree)](#api), [void gui_cube_auto_rotation_by_y(gui_cube_t *this, uint32_t internal_ms, float degree)](#api) and [void gui_cube_auto_rotation_by_z(gui_cube_t *this, uint32_t internal_ms, float degree)](#api).


## Example
```c
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"

void page_tb_cube(void *parent)
{
    gui_cube_imgfile_t imgfile =
    {
        .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
        .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
        .data_addr.data_addr_front = ACTIVITY_BIN,
        .data_addr.data_addr_back = WEATHER_BIN,
        .data_addr.data_addr_up =  HEARTRATE_BIN,
        .data_addr.data_addr_down = CLOCKN_BIN,
        .data_addr.data_addr_left = MUSIC_BIN,
        .data_addr.data_addr_right = QUICKCARD_BIN
    };
    // gui_cube_imgfile_t imgfile =
    // {
    //     .src_mode[0] = IMG_SRC_FILESYS, .src_mode[1] = IMG_SRC_FILESYS, .src_mode[2] = IMG_SRC_FILESYS,
    //     .src_mode[3] = IMG_SRC_FILESYS, .src_mode[4] = IMG_SRC_FILESYS, .src_mode[5] = IMG_SRC_FILESYS,
    //     .img_path.img_path_front = "Activity.bin",
    //     .img_path.img_path_back = "Weather.bin",
    //     .img_path.img_path_up = "HeartRate.bin",
    //     .img_path.img_path_down = "Clockn.bin",
    //     .img_path.img_path_left = "Music.bin",
    //     .img_path.img_path_right = "QuickCard.bin"
    // };
    gui_cube_t *cube4 = gui_cube_create(parent, "cube", &imgfile, 0, 0);
    gui_cube_auto_rotation_by_y(cube4, 100, 5.5f);
    gui_cube_set_mode(cube4, CUBE_SIDE_DOWN, IMG_SRC_OVER_MODE);
    gui_cube_set_size(cube4, 100);
    gui_cube_set_center(cube4, 200, 200);
}
```
<br/>
<div style="text-align: center"><img width= "300" src="https://foruda.gitee.com/images/1699955249631031248/bdc522a1_13671125.gif "></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_cube.h

```

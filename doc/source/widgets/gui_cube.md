# Cube
<br/>

## Overview
The cube is a 3D display widget that show a rotating cube with six faces, each capable of displaying distinct images.

## Usage
### Create a cube
[gui_cube_create(parent, name, addr, x, y, w, h)](#api) creates a cube widget. This `addr` is an array of image sources for the six faces of a cube.

### Set size
By default, the cube's size is `dc->fb_height / 8.0`. If you want to change the size set [gui_cube_set_size(cube, size)](#api). Note this `size` is the 1/2 length of cube edge.

### Set center
By default, the center is `((dc->fb_width - size) / 2.0f,dc->fb_width - size) / 2.0f)`. You can use [gui_cube_set_center(cube, c_x, c_y)](#api) to set the center coordinates of the cube.

## Example

```c
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"

void page_tb_cube(void *parent)
{
    uint8_t *array[6] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};

    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
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
.. doxygenfunction:: gui_cube_create
.. doxygenfunction:: gui_cube_set_center
.. doxygenfunction:: gui_cube_set_size
```

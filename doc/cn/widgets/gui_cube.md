# 立方体

立方体是一个3D显示控件，它可以展示六个面的旋转立方体，每个面都能够显示不同的图像。

## 用法
### 创建控件
[gui_cube_t *gui_cube_create(void *parent,  const char *name, gui_cube_imgfile_t *img_file, int16_t x, int16_t y)](#api)创建一个cube控件。其中，`img_file`是一个包含了每个面图像源的结构体，支持内存地址或者文件系统路径。通过将`IMG_SOURCE_MODE_TYPE src_mode[6]`设置成`IMG_SRC_MEMADDR`或`IMG_SRC_FILESYS`来选择内存地址还是文件系统路径访问图像。

### 设置大小
立方体控件的默认大小是`dc->fb_height / 8.0`，可以通过[void gui_cube_set_size(cube, size)](#api)改变大小。注意`size`是立方体的1/2边长。

### 设置中心
立方体控件的默认中心是`((dc->fb_width - size) / 2.0f,dc->fb_width - size) / 2.0f)`。使用[void gui_cube_set_center(cube, c_x, c_y)](#api)改变立方体的中心坐标。

### 设置图像模式
立方体控件默认图像混合模式是`IMG_FILTER_BLACK`（*滤黑，即图像中像素值为0的像素不写入屏幕刷新帧缓冲区*），可以通过[void gui_cube_set_mode(gui_cube_t *cube, T_CUBE_SIDE_TYPE cube_side, BLEND_MODE_TYPE mode)](#api)来改变图像混合模式。

### 设置立方体图像
可以通过[void gui_cube_set_img(gui_cube_t *cube, gui_cube_imgfile_t *img_file)](#api)来设置立方体展示的图像。

### 设置旋转角度
立方体的旋转角度可以通过以下API设置，[void gui_cube_auto_rotation_by_x(gui_cube_t *this, uint32_t internal_ms, float degree)](#api)，[void gui_cube_auto_rotation_by_y(gui_cube_t *this, uint32_t internal_ms, float degree)](#api)以及[void gui_cube_auto_rotation_by_z(gui_cube_t *this, uint32_t internal_ms, float degree)](#api)。


## 示例
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
<div style="text-align: center"><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/widgets/cube.gif"></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_cube.h

```

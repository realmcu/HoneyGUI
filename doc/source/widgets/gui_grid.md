# Grid
<br>

## Overview
The Grid Widget allows you to display widget set in a grid view.

## Usage

### Create grid widget
You can use [gui_grid_create(parent, x, y, row, col, gap, col, gap_row)](#api) to create a grid widget, Populate the grid with controls in the order they were created

### Set grid style
There are two style in grid widget [gui_grid_style(grid, style)](#api)

```
enum gui_grid_style
{
    GRID_CLASSIC,         //!< Evenly layout vertically and horizontally
    GRID_SCALE,           //!< scale by slide
};
```

## Example

### classic style


```cpp
#include <gui_obj.h>
#include <gui_grid.h>
#include "gui_magic_img.h"
#include "root_image_hongkong/ui_resource.h"

void page_tb_grid(void *parent)
{
    void *array[] = {CLOCK_BIN, EMAIL_BIN, SPORT_BIN, SLEEP_BIN, MAP_BIN};
    gui_grid_t *grid = gui_grid_create(parent, 0, 0, 2, 3, 210, 100);
    gui_grid_style(grid, GRID_CLASSIC);
    for (size_t i = 0; i < 5; i++)
    {
        gui_img_creat_from_mem(grid, "1", array[i], 0, 0, 0, 0);
    }
}
```
<br>
<center><img width = "300" src= "https://foruda.gitee.com/images/1693896763454036220/6c0a498b_10088396.png"/></center>
<br>


## API 


```eval_rst

.. doxygenfile:: gui_grid.h

```
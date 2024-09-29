# Grid

The Grid Widget allows a display widget set in a grid view.

## Usage

### Create Grid Widget
Use [gui_grid_create(void *parent, int16_t x, int16_t y, int16_t row, int16_t col, uint16_t gap_col, uint16_t gap_row)](#gui_grid_create) to create a grid widget. Populate the grid with controls in the order they were created.

### Set Grid Style
There are two styles in the grid widget [gui_grid_style(gui_grid_t *grid, T_GUI_GRID_STYLE style)](#gui_grid_style).

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_grid.h
   :language: c
   :start-after: /* T_GUI_GRID_STYLE enum start*/
   :end-before: /* T_GUI_GRID_STYLE enum end*/
```

## Example

### Classic Style


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


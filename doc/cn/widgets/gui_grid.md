# 网格

网格组件可以在网格视图中显示组件集。

## 用法

### 创建网格组件
开发者可以通过[gui_grid_create(parent, x, y, row, col, gap, col, gap_row)](#gui_grid_create) 创建网格组件, 按照创建的顺序用控件填充网格。

### 设置网格样式
开发者可以使用[gui_grid_style(grid, style)](#gui_grid_style) 进行设置样式，总共有以下两种样式。

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_grid.h
   :language: c
   :start-after: /* T_GUI_GRID_STYLE enum start*/
   :end-before: /* T_GUI_GRID_STYLE enum end*/
```

## 示例

### 经典样式

<details> <summary>代码</summary>

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
</details>
<br>
<center><img width = "300" src= "https://foruda.gitee.com/images/1693896763454036220/6c0a498b_10088396.png"/></center>
<br>


## API 


```eval_rst

.. doxygenfile:: gui_grid.h

```
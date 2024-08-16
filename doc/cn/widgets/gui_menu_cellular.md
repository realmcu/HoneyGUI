# 蜂窝菜单

蜂窝菜单组件以蜂窝形状显示一组图标。

## 用法

### 创建蜂窝菜单组件

要创建蜂窝菜单组件，使用 [gui_menu_cellular_create(void *parent, int icon_size, uint32_t *icon_array[],int array_size)](#gui_menu_cellular_create) 函数。用户需要提供代表图标集的图像文件数组。
### 配置偏移

使用 [void gui_menu_cellular_offset(gui_obj_t *menu_cellular, int offset_x, int offset_y)](#gui_menu_cellular_create) 函数可以调整水平和垂直偏移以获得更好的显示效果。

## 示例
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/menu_cellular.gif"  /></div>
<br>

```c
#include "gui_menu_cellular.h"
static void app_menu(gui_app_t *app)
{
    /**
     * @link https://docs.realmcu.com/Honeygui/latest/widgets/gui_menu_cellular.html#example
    */
    uint32_t *array[] =
    {
        I4500009_BIN, 
        I4500011_BIN, 
        I4500015_BIN, 
        I4500020_BIN, 
        I4500028_BIN, 
        I4500034_BIN, 
        I4500035_BIN, 
        I4500037_BIN, 
        I4500040_BIN, 
        I4500042_BIN, 
        I4500047_BIN, 
        I4500049_BIN, 
        I4500052_BIN, 
        I4500064_BIN, 
        I4500009_BIN, 
        I4500011_BIN, 
        I4500015_BIN, 
        I4500020_BIN, 
        I4500028_BIN, 
        I4500034_BIN, 
        I4500035_BIN, 
        I4500037_BIN, 
        I4500040_BIN, 
        I4500042_BIN, 
        I4500047_BIN, 
        I4500049_BIN, 
        I4500052_BIN, 
        I4500064_BIN, 
        I4500009_BIN, 
        I4500011_BIN, 
        I4500015_BIN, 
        I4500020_BIN, 
        I4500028_BIN, 
        I4500034_BIN, 
        I4500035_BIN, 
        I4500037_BIN, 
        I4500040_BIN, 
        I4500042_BIN, 
        I4500047_BIN, 
        I4500049_BIN, 
        I4500052_BIN, 
        I4500064_BIN, 
        I4500009_BIN, 
        I4500011_BIN, 
        I4500015_BIN, 
        I4500020_BIN, 

    };
    gui_menu_cellular_t *cell = gui_menu_cellular_create(GUI_APP_ROOT_SCREEN, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    gui_menu_cellular_offset((void *)cell, -36, -216);
}
```


<span id = "gui_menu_cellular_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_menu_cellular.h

```

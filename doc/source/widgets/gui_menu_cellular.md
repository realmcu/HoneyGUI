# Menu cellular
<br>

## Overview

The menu cellular widget display a icons set as a honeycomb shape menu.

## Usage

### Create menu cellular widget

Creating function is [gui_menu_cellular_create(void *parent, int icon_size, uint32_t *icon_array[],int array_size)](#gui_menu_cellular_create). User needs to enter icons set's image file array.

## Example
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1712471972466315426/a615a7a8_10088396.png " width = "300" /></div>
<br>

```c
#include "gui_menu_cellular.h"
static void design_app_watch_ui(gui_app_t *app)
{
    static uint32_t *array[] = 
    {
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN, 
        ICON_PHONE_ON_BIN, 
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN, 
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN, 
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN, 
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,
        ICON_PHONE_ON_BIN,

    };
    gui_menu_cellular_create(&(app->screen), 100, array, sizeof(array)/sizeof(uint32_t *));


}
```


<span id = "gui_menu_cellular_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_menu_cellular_create.h

```

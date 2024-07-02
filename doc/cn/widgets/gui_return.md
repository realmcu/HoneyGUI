# Return
<br>

## Overview

The bubble-shaped curve that can be dragged out on the left side of the screen is used to perform a return operation.

## Usage

### Create a return widget

Creating function is [gui_return_create](#gui_return_create). Users need to enter curve animation frame images file array ,and the return operation function.

## Example
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1713248172846272975/fd33e8c2_10088396.gif"  /></div>
<br>


```eval_rst

.. note::
  Please refer to ```gui_engine\example\screen_454_454\gui_menu\apps.c``` for details.

```




```c
const uint32_t *gui_app_return_array[] =
{
    PATH04_BIN,
    PATH05_BIN,
    PATH07_BIN,
    PATH08_BIN,
    PATH09_BIN,
    PATH11_BIN,
    PATH12_BIN,
    PATH14_BIN,
    PATH15_BIN,
    PATH16_BIN,
    PATH18_BIN,
    PATH19_BIN,
    PATH20_BIN,
    PATH22_BIN,
    PATH23_BIN,
    PATH24_BIN,
    PATH25_BIN,
};
gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)cell);
```


<span id = "gui_return_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_return.h

```

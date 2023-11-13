# **Curtain**

## Overview

The curtain widget allows you to create a curtain sliding effect in five directions.
Before creating a curtain widget, you must select Create Curtainview widget.

## Features

Curtain widget can support the following features.

- Five selectable directions

## Usage

- ### Create curtain widget

Use this functions to create curtain widget

```c
    gui_curtainview_t *ct = gui_curtainview_create(parent, "ct", 0, 0, 368, 448);
    gui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
```

- ### Select curtain orientation

There are five directions in curtain widget.

```c
typedef enum CURTAIN_ORIENTATION
{
    CURTAIN_UNDEFINED,
    CURTAIN_UP,
    CURTAIN_DOWN,
    CURTAIN_LEFT,
    CURTAIN_RIGHT,
    CURTAIN_MIDDLE,
} gui_curtain_enum_t;
```

<details> <summary>Example code</summary>

```c
#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_canvas.h"

void page_tb_clock(void *parent)
{
    gui_curtainview_t *ct = gui_curtainview_create(parent, "ct", 0, 0, 368, 448);
    GET_BASE(ct)->cover = true;
    gui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
    gui_curtain_t *ct_left = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_LEFT, 0.65f);

    gui_curtain_t *ct_card = gui_curtain_create(ct, "card", 0, 0, 368, 448, CURTAIN_DOWN, 1);

    extern void page_ct_clock(void *parent);
    extern void page_ct_sidebar(void *parent);
    extern void tabview_up_design(void *parent_widget);
    extern void curtain_down_design(void *parent_widget);
    page_ct_clock(ct_clock);
    page_ct_sidebar(ct_left);
    tabview_up_design(ct_control0);
    curtain_down_design(ct_card);
}
```

</details>

## Example display

### Curtain middle

<div align=center><img src="https://foruda.gitee.com/images/1699598241164687669/dfa390fd_10641540.png"></div>

### Curtain left

<div align=center><img src="https://foruda.gitee.com/images/1699598101922668537/42e53db8_10641540.png"></div>

### Curtain down

<div align=center><img src="https://foruda.gitee.com/images/1699598123896262065/f1ca11fb_10641540.png"></div>

<iframe src="https://drive.google.com/file/d/1ldpbh_wWDRebAIMNhPelFBF98Der49An/preview" width="640" height="480" allow="autoplay"></iframe>

## API

```eval_rst

.. doxygenfile:: gui_curtain.h

```

- ### Struct

```c
typedef struct gui_curtain
{
    gui_obj_t base; 
    gui_curtain_enum_t orientation;
    float scope;
    void (*ctor)(struct gui_curtain *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope); 
} gui_curtain_t;
```

- ### Function

    gui_curtain_create()

```c
gui_curtain_create(void *parent, const char *filename, int16_t x, int16_t y,int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope)
```

|params  | description  |
|--|--|
|`parent`|the father widget the grid nested in|
|`filename`|the widget name|
|`x`|the X-axis coordinate|
|`y`|the Y-axis coordinate|
|`w`|the widget width|
|`h`|the widget height|
|`orientation`|the curtain orientation|
|`scope`|the curtain scope||
|`return`|the widget object pointer|

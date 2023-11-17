# Curtain
<br>

## Overview

The curtain widget allows you to create a curtain sliding effect in five directions.
Before creating a curtain widget, you must select create curtainview widget.

## Features

Curtain widget can support the following features.

- Five selectable directions

## Usage

### Create curtain widget

Use [gui_curtain_create](#gui_curtain_create) to create curtain widget

```c
gui_curtainview_t *ct = gui_curtainview_create(parent, "ct", 0, 0, 368, 448);
ui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
```

### Select curtain orientation

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

## Example

<details> <summary>code</summary>

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
<br>


<div align=center><img src="https://foruda.gitee.com/images/1699869962427925475/4a382788_10641540.png"></div>


<span id="gui_curtain_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_curtain.h
```

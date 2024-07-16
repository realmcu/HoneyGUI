# Curtain

The curtain widget enables you to create curtains in five directions: up, down, left, right, and middle. (refer to [T_GUI_CURTAIN_ENUM](#T_GUI_CURTAIN_ENUM)). Before creating the curtain widget, you must first create a curtainview widget to accommodate the curtain.

## Usage

### Create curtain widget

If you wish to create a curtain widget with a defined direction, you may opt to utilize the [gui_curtain_create(parent, filename, x, y, w, h, orientation, scope)](#gui_curtain_create) function.
`Orientation` field offers various directions for the curtain, allowing for the creation of a middle or right curtain widget.
`Scope` refers to the curtain range that can be extended, which falls between 0 and 1

<span id = "T_GUI_CURTAIN_ENUM">

### Orientation

</span>

There are five orientations in curtain widget.

```c
typedef enum CURTAIN_ORIENTATION
{
    CURTAIN_UNDEFINED,
    CURTAIN_UP,
    CURTAIN_DOWN,
    CURTAIN_LEFT,
    CURTAIN_RIGHT,
    CURTAIN_MIDDLE,
} T_GUI_CURTAIN_ENUM;
```



## Example

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

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699869962427925475/4a382788_10641540.png" width = "300" /></div>
<br>

<span id="gui_curtain_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_curtain.h

```
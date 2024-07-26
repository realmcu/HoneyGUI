# 幕布
<br>

幕布控件可以创建5个不同方向的幕布，分别是上、下、左、右、中间 (参考[T_GUI_CURTAIN_ENUM](#T_GUI_CURTAIN_ENUM))，创建幕布控件之前，必须先创建幕布视图控件(`curtainview`)来容纳幕布。

## 使用方法

### 创建幕布控件

如果想要创建带有方向的幕布控件，可以使用[gui_curtain_create(parent, filename, x, y, w, h, orientation, scope)](#gui_curtain_create) 函数来创建。
`Orientation` 表示方向，可以创建上、下、左、右、中间等五个不同方向的幕布控件。
`Scope` 表示幕布的扩展范围，范围为0到1。

<span id = "T_GUI_CURTAIN_ENUM">

### 幕布控件方向

</span>

幕布控件有5个方向，分别是上、下、左、右、中间。

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

## 示例

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

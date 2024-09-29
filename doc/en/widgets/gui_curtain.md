# Curtain

The curtain widget enables you to create curtains in five directions: up, down, left, right, and middle (refer to [T_GUI_CURTAIN_ENUM](#T_GUI_CURTAIN_ENUM)). Before creating the curtain widget, you must first create a curtainview widget to accommodate the curtain.

## Usage

### Create Curtain Widget

If you want to create a curtain widget with a defined direction, you may opt to utilize the [gui_curtain_create(parent, filename, x, y, w, h, orientation, scope)](#gui_curtain_create) function.
`orientation` represents direction and can create curtain controls in five different directions: up, down, left, right, and center.
`Scope` refers to the curtain range that can be extended, which falls between 0 and 1.

<span id = "T_GUI_CURTAIN_ENUM">

### Orientation

</span>

There are five orientations in curtain widget.

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_curtainview.h
   :language: c
   :start-after: /* T_GUI_CURTAIN_ENUM start*/
   :end-before: /* T_GUI_CURTAIN_ENUM end*/

```

## Example

```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_tb_clock.c
   :language: c
   :start-after: /* curtain example start*/
   :end-before: /* curtain example end*/

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
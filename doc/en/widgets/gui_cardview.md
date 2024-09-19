# Cardview

Card effect container widget, which can nest cards. You can switch cards by swiping.

<br>
<center><img width="500" src= "https://foruda.gitee.com/images/1700123394899348792/914aadf9_10641540.png "></center>
<br>

## Usage

### Create Widget
Using [gui_cardview_create(parent,  name, x, y, w, h)](#gui_cardview_create) to create a cardview widget.

### Callback
When the state of the cardview changes, it triggers a callback, and the callback function [gui_cardview_status_cb(this, cb)](#gui_cardview_status_cb) can be used to perform specific actions.

### Set Style
There are five styles in the cardview widget [gui_cardview_set_style(this, style)](#gui_cardview_set_style).

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/
   
```

### Set Center Alignment
This function [gui_cardview_alignment(this, align_hight)](#gui_cardview_alignment) sets the card to automatic center alignment.

### Set Bottom Space
This function [gui_cardview_set_bottom_space(this, bottom_space)](#gui_cardview_set_bottom_space) sets the bottom spacing of the card.

### Y-axis Slide Callback
The event of the card container sliding on the Y-axis can add the corresponding callback function [gui_cardview_up(this, callback, parameter)](#gui_cardview_up) and [gui_cardview_down(this, callback, parameter)](#gui_cardview_down).

## Example
Please refer to the section: [card](./gui_card.md)

## API

```eval_rst

.. doxygenfile:: gui_cardview.h

```

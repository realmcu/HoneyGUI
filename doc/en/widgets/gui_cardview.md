# Cardview

Card effect container widget, which can nest card. You can switch cards by swiping.

<br>
<center><img width="500" src= "https://foruda.gitee.com/images/1700123394899348792/914aadf9_10641540.png "></center>
<br>

## Usage

### Create widget
Using [gui_cardview_create(parent,  name, x, y, w, h)](#api) to create cardview widget

### Callback
When the state of the cardview changes, it triggers a callback, and you can use the callback function [gui_cardview_status_cb(this, cb)](#api) to do what you want.

### Set style
There are five style in cardview widget [gui_cardview_set_style(this, style)](#api)

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/
   
```

### Set center alignment
You can use this function [gui_cardview_alignment(this, align_hight)](#api)to set the card automatic center alignment.

### Set bottom space
You can use this function [gui_cardview_set_bottom_space(this, bottom_space)](#api)to set the bottom spacing of the card.

### Y-axis slide callback
The event of the card container sliding in the Y-axis can add the corresponding callback function[gui_cardview_up(this, callback, parameter)](#api) and [gui_cardview_down(this, callback, parameter)](#api).

## Example
Please refer to the page below:
```eval_rst
`card`_    

.. _card: gui_card.html

```

## API

```eval_rst

.. doxygenfile:: gui_cardview.h

```

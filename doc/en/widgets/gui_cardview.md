# Cardview
<br>

## Overview
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

```
typedef enum t_slide_style
{
    CLASSIC,
    REDUCTION,
    FADE,
    REDUCTION_FADE,
    STACKING,
} T_SLIDE_STYLE;
```

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

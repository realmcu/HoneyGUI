# Curtainview
<br>

## Overview

The gui_curtainview widget provides an extended area for the developer to place application-related information, such as the application menu or the information for the application.
As illustrated in Figure, the gui_curtainview can comprise of four curtains that can be positioned from the up/down/right/left orientation, or they can be situated in the central area.
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1700114998989746788/e5140991_10641540.png" /></div>
<br>

## Usage

### Create curtainview widget

The function [gui_curtainview_create(parent, filename, x, y, w, h)](#gui_curtainview_create) is utilized to create a container that is capable of housing the curtain widget.

### Callback

Once the curtain extension is finished, you can configure certain callback events through the use of  [gui_curtainview_set_done_cb (this, cb)](#gui_curtainview_create).

## Example

Please refer to the page below:

```eval_rst
`curtain`_

.. _curtain: gui_curtain.html

```

<span id = "gui_curtainview_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_curtainview.h
.. doxygenfunction:: gui_curtainview_create
.. doxygenfunction:: gui_curtainview_set_done_cb
```

# Screen

<br>

## Overview

The screen widget is the root node of a control tree. The screen coordinate system is set as follows. The origin of the polar coordinates is the negative direction of the Y axis, and the positive direction of the polar coordinates is clockwise:
<br>

<center><img src="https://foruda.gitee.com/images/1669963637201743951/88578c71_10088396.jpeg" /></center>

## Usage

### Add event

You can add an event of widget by this api [gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data)](#api).
`obj` is the selected widget, `event_cb` is the switching events, `filter` is the way how to trigger event, and `user_data` is the data to transmit.

### Set event
You can set an event of widget by this api [gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code)](#api)(this api only for Widget, not for Application).

### Free widget

Using [gui_tree_free(void *obj)](#api) to free the widget tree recursively from the root to the leaves(Preorder traversal).

### Print widget

Using [gui_tree_print(gui_obj_t *obj)](#api) to print the widget tree recursively from the root to the leaves.

### Hide/Show widget

You can show or hide the widget by these apis: [gui_tree_show(gui_obj_t *obj)](#api) and [gui_tree_not_show(gui_obj_t *obj, bool enable)](#api).

### Set object show or not

You can set the object show or not with [gui_obj_show(void *obj, bool show_info)](#api).

### Get root

You can get the root object of tree using [gui_obj_t *gui_tree_get_root(gui_obj_t *obj)](#api).

### Get child

To get the child object of tree by this api [gui_get_child_handle(gui_obj_t *obj, obj_type_t child_type)](#api).

### Get cover

To get cover widget by this api [gui_tree_get_cover(gui_obj_t *obj, obj_type_t target, bool *rst)](#api).
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfunction:: gui_obj_add_event_cb
.. doxygenfunction:: gui_obj_event_set
.. doxygenfunction:: gui_tree_free
.. doxygenfunction:: gui_tree_print
.. doxygenfunction:: gui_tree_not_show
.. doxygenfunction:: gui_tree_show
.. doxygenfunction:: gui_obj_show
.. doxygenfunction:: gui_tree_get_root
.. doxygenfunction:: gui_get_child_handle
.. doxygenfunction:: gui_tree_get_cover
```

# Watch Gradient Spot

The watch gradientm spot widget allows you to create a pointer watchface with dynamic water drops in the center.

## Usage

### Create Widget

You can create a watch gradient spot widget by calling the [gui_watch_gradient_spot_create(parent, name, x, y, w, h)](#gui_watch_gradient_spot_create).

### Set Center

The center position of the widget can be freely set by using the [gui_watch_gradient_spot_set_center(this, c_x, c_y)](#gui_watch_gradient_spot_set_center) function to change the center position, where `c_x` and `c_y` are the coordinate points of the central position.

## Example

<details> <summary>code</summary>

```eval_rst
.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* watchface_watch_gradient_spot demo start*/
   :end-before: /* watchface_watch_gradient_spot demo end*/

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_tablist.c
   :language: c
   :start-after: /* watch_gradient_spot start*/
   :end-before: /* watch_gradient_spot end*/

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* callback_touch_long start*/
   :end-before: /* callback_touch_long end*/

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* page_ct_clock start*/
   :end-before: /* page_ct_clock end*/
```

</details>

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931734086261442/f0690691_10641540.png" width = "300" /></div>
<br>

<span id = "gui_watch_gradient_spot_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_watch_gradient_spot.h

```

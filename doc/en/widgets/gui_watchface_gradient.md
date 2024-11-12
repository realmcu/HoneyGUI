# Watchface Gradient

The watchface gradient widget allows you to create a pointer watchface with a scanning effect for each pointer.

## Usage

### Create Widget

To reate a new watchface gradient widget, you can call the function [gui_watchface_gradient_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_watchface_gradient_create) to achieve it.

## Example

<details> <summary>code</summary>

```eval_rst
.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* watchface_watch_gradient_spot demo start*/
   :end-before: /* watchface_watch_gradient_spot demo end*/

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_tablist.c
   :language: c
   :start-after: /* watchface start*/
   :end-before: /* watchface end*/

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
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931787525761928/f8a2202f_10641540.png" width = "400" /></div>
<br>

<span id = "gui_watchface_gradient_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_watchface_gradient.h

```

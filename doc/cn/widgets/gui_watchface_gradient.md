# 表盘渐变 (Watchface Gradient)

表盘渐变是指具有指针扫描效果的指针表盘控件。

## 用法

### 创建控件

使用 [gui_watchface_gradient_create(parent, name, x, y, w, h)](#gui_watchface_gradient_create) 函数创建具有扫描效果的指针表盘控件。

## 示例

<details> <summary>代码</summary>

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
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931787525761928/f8a2202f_10641540.png" width = "300" /></div>
<br>

<span id = "gui_watchface_gradient_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_watchface_gradient.h

```

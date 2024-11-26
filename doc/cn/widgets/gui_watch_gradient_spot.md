# 监视点渐变 (Watch Gradient Spot)

监视点渐变是一个具有动态水滴效果的指针表盘控件。

## 用法

### 创建控件

使用 [gui_watch_gradient_spot_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_watch_gradient_spot_create) 函数创建一个具有动态水滴效果的指针表盘控件。

### 设置中心位置

控件的中心位置可以自由设置，使用[gui_watch_gradient_spot_set_center(gui_watch_gradient_spot_t *this, float c_x, float c_y)](#gui_watch_gradient_spot_set_center) 函数改变中心位置，其中 `c_x` 和 `c_y` 为中心位置的坐标点。

## 示例

<details> <summary>代码</summary>

```eval_rst
.. literalinclude:: ../../../realgui/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* watchface_watch_gradient_spot demo start*/
   :end-before: /* watchface_watch_gradient_spot demo end*/

.. literalinclude:: ../../../realgui/example/screen_448_368/app_tablist.c
   :language: c
   :start-after: /* watch_gradient_spot start*/
   :end-before: /* watch_gradient_spot end*/

.. literalinclude:: ../../../realgui/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* callback_touch_long start*/
   :end-before: /* callback_touch_long end*/

.. literalinclude:: ../../../realgui/example/screen_448_368/app_ct_clock.c
   :language: c
   :start-after: /* page_ct_clock start*/
   :end-before: /* page_ct_clock end*/
```

</details>




<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931734086261442/f0690691_10641540.png" width = "400" /></div>
<br>

<span id = "gui_watch_gradient_spot_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_watch_gradient_spot.h

```

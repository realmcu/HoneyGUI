# 幕布 (Curtain)
<br>

幕布控件可以创建5个不同方向的幕布，分别是上、下、左、右、中间 (参考[T_GUI_CURTAIN_ENUM](#T_GUI_CURTAIN_ENUM))，创建幕布控件之前，必须先创建幕布视图控件 (curtainview) 来容纳幕布。

## 用法

### 创建幕布控件

如果想要创建带有方向的幕布控件，可以使用[gui_curtain_create(parent, filename, x, y, w, h, orientation, scope)](#gui_curtain_create) 函数来创建。
`orientation` 表示方向，可以创建上、下、左、右、中间等五个不同方向的幕布控件。
`Scope` 表示幕布的扩展范围，范围为0到1。

<span id = "T_GUI_CURTAIN_ENUM">

### 幕布控件方向

</span>

幕布控件有5个方向。

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_curtainview.h
   :language: c
   :start-after: /* T_GUI_CURTAIN_ENUM start*/
   :end-before: /* T_GUI_CURTAIN_ENUM end*/

```

## 示例

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

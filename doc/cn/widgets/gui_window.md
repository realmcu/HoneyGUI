# 窗口 (window)
<br>

## 概述

此控件为开发者提供了一个虚拟区域，用于放置应用程序所需的控件。开发者可以根据需求创建相对于屏幕的空间。
例如，图-1 创建了一个与屏幕尺寸相同的区域，而开发者也可以创建不同尺寸的空间，如图-2 所示。

<center><img src="https://foruda.gitee.com/images/1701081169144847122/2f0a8469_13671147.png" /></center>
<br>
<center>图-1</center>
<br>

<center><img src="https://foruda.gitee.com/images/1701081183476854396/dec93062_13671147.png" /></center>
<br>
<center>图-2</center>
<br>

如图-3 所示的小部件将以窗口小部件的左上角作为初始坐标。
<br>

<center><img src="https://foruda.gitee.com/images/1701081206134160709/80ae8874_13671147.png" /></center>
<br>
<center>图-3</center>
<br>

## 使用方法

### 创建窗口部件

你可以通过这个 API [`gui_win_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)`](#api)创建一个窗口控件。
其中 `w/h` 是窗口控件的宽度和高度。
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_win.h

```

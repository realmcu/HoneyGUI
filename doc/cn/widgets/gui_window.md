# 窗口

此控件为开发者提供了一个虚拟区域，用于放置应用程序所需的控件。开发者可以根据需求创建相对于屏幕的空间。
例如，图1创建了一个与屏幕尺寸相同的区域，而开发者也可以创建不同尺寸的空间，如图2所示。

<center><img src="https://foruda.gitee.com/images/1701081169144847122/2f0a8469_13671147.png" /></center>
<center>图1</center>
<br>

<center><img src="https://foruda.gitee.com/images/1701081183476854396/dec93062_13671147.png" /></center>
<center>图2</center>
<br>

下图中的控件将以窗口控件的左上角作为初始坐标。
<br>

<center><img src="https://foruda.gitee.com/images/1701081206134160709/80ae8874_13671147.png" /></center>
<br>

## 使用方法

### 创建窗口控件

可以通过 [`gui_win_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)`](#api) 该API创建一个窗口控件，
其中 `w/h` 是窗口控件的宽度和高度。
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_win.h

```

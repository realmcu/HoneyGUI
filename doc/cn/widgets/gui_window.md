# 窗口 (Win)

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

可以通过 [gui_win_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_win_create) 函数创建一个窗口控件。

### 设定动画

开发者可以调用函数 [gui_win_set_animate(gui_win_t *_this, uint32_t dur, int repeat_count, void *callback, void *p)](#gui_win_set_animate) 来设置动画并在相应的回调函数中实现动画效果。

### 注册回调函数

使用以下的函数来给窗口控件注册左/右/上/下滑动事件：[gui_win_left(gui_win_t *_this, void *callback, void *parameter)](#gui_win_left) , [gui_win_right(gui_win_t *_this, void *callback, void *parameter)](#gui_win_right) , [gui_win_up(gui_win_t *_this, void *callback, void *parameter)](#gui_win_up) , [gui_win_down(gui_win_t *_this, void *callback, void *parameter)](#gui_win_down)。<br/>
并且也可以使用以下的函数来给窗口控件注册按下/释放/长按/点击事件：[gui_win_press(gui_win_t *_this, void *callback, void *parameter)](#gui_win_press) , [gui_win_release(gui_win_t *_this, void *callback, void *parameter)](#gui_win_release) , [gui_win_long(gui_win_t *_this, void *callback, void *parameter)](#gui_win_long) , [gui_win_click(gui_win_t *_this, void *callback, void *parameter)](#gui_win_click)。

### 设置触摸保持状态

使用函数 [gui_win_hold_tp(gui_win_t *_this, bool hold_tp)](#gui_win_hold_tp) 来设置触摸保持'hold_tp'的状态。

### 获取进度

通过函数 [gui_win_get_animation_progress_percent(gui_win_t *win)](#gui_win_get_animation_progress_percent) 来获取动画进度。

### 缩放设定

通过函数 [gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical)](#gui_win_set_scale_rate) 设置窗口控件在水平方向和垂直方向的缩放比例。

### 允许/禁止

使用函数 [gui_win_set_scope(gui_win_t *win, bool enable)](#gui_win_set_scope) 来设定允许或禁止窗口控件。

### 不透明度

可以使用函数 [gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value)](#gui_win_set_opacity) 来设定窗口控件的不透明度。

### 动画检查

使用函数 [gui_win_is_animation_end_frame(gui_win_t *win)](#gui_win_is_animation_end_frame) 来检查动画是否到了最后一帧。

### 启动动画

使用函数 [gui_win_start_animation(gui_win_t *win)](#gui_win_start_animation) 来启动动画。

### 停止动画

通过函数 [gui_win_stop_animation(gui_win_t *win)](#gui_win_stop_animation) 来停止动画。

### 准备

窗口控件的准备处理函数 [gui_win_prepare(gui_obj_t *obj)](#gui_win_prepare)。

### 添加动画

使用函数 [gui_win_append_animate(gui_win_t *win, uint32_t dur, int repeat_count, void *callback, void *p, const char *name)](#gui_win_append_animate) 将动画添加到GUI窗口。
<br>

## API

```eval_rst

.. doxygenfile:: gui_win.h

```

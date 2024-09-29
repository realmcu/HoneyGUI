# 画布（Canvas）

画布是一个使用nanovg绘制几何图形的控件。

```{限制}
打开帧缓冲区需要足够的内存。
```

## 用法
### 创建控件
使用 [gui_canvas_create(void *parent, const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_canvas_create) 创建一个画布。其中，`addr` 可以是一个空指针（NULL）。

### 添加回调函数
使用 [gui_canvas_set_canvas_cb(gui_canvas_t *this_widget, void (*cb)(gui_canvas_t *this_widget))](#gui_canvas_set_canvas_cb) 设置绘制具体图形的回调函数。

## 示例
### 圆角矩形

以下是一个绘制三个不同颜色的圆角矩形的简单例子。
- 其中，三种颜色参考 [Colors' RGB Data](https://www.rapidtables.com/web/color/RGB_Color.html)。
- ```firebrick```
- ```olive drab```
- ```dodger blue``` 设置透明度为100。

```c
#include "gui_canvas.h"
static void canvas_cb(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 10, 10, 348, 200, 30);
    nvgFillColor(canvas->vg, nvgRGB(178,34,34));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
    nvgRoundedRect(canvas->vg, 10, 220, 348, 200, 30);
    nvgFillColor(canvas->vg, nvgRGB(107,142,35));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
    nvgRoundedRect(canvas->vg, 110, 125, 148, 200, 30);
    nvgFillColor(canvas->vg, nvgRGBA(30,144,255, 100));
    nvgFill(canvas->vg);
}
static void app_ui_design(gui_app_t *app)
{
    gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb);
}
```
<br/>
<div style="text-align: center"><img width= "300" src="https://foruda.gitee.com/images/1698649650262539854/8b1a974f_10088396.png"></div>
<br/>

### 圆弧动画

以下是一个绘制圆弧动画的例子，其中，屏幕刷新的每一帧都会触发 ```arc_cb``` 函数。

```c
#include "math.h"
#include "gui_canvas.h"
static void arc_cb(gui_canvas_t *canvas)
{
    static float  progress;
    progress +=0.01;
    nvgArc(canvas->vg, 368/2, 448/2, 150, 0, 3.14*(sinf(progress)+1), NVG_CCW);
    nvgStrokeWidth(canvas->vg, 20);
    nvgStrokeColor(canvas->vg, nvgRGB(178,34,34));
    nvgStroke(canvas->vg);
}
static void app_ui_design(gui_app_t *app)
{
    gui_canvas_t *canvas = gui_canvas_create(&(app->screen), "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, arc_cb);
}
```
<br/>
<div style="text-align: center"><img width= "300" src="https://docs.realmcu.com/HoneyGUI/image/widgets/canvas_arc.gif"></div>
<br/>

## API
### Nanovg API
请参考以下地址：
- <a href="https://openplanet.dev/docs/tutorials/nanovg-introduction">Nanovg Introduction</a>
- [Nanovg Github](https://github.com/memononen/nanovg)

### RealGUI API

```eval_rst

.. doxygenfile:: gui_canvas.h

```

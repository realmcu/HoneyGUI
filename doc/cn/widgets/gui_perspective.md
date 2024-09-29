# 透视 (Perspective)

透视控件是一个六面体形状的控件，由六个标签页作为六个面。当创建好透视控件后，该控件会自动旋转来显示不同的标签页，在旋转的同时也可以点击不同的标签页实现快速跳转到当前标签页。

## 用法

### 创建透视控件

使用 [gui_perspective_t *gui_perspective_create(void *parent,  const char *name, gui_perspective_imgfile_t *img_file, int16_t x, int16_t y)](#gui_perspective_create) 函数创建透视控件。`img_file` 是一个包含六个标签页的图像资源的结构体。创建透视控件时，六个标签页的图片资源可以使用内存地址或文件路径，`gui_perspective_imgfile_t` (参考如下结构体) 中的字段 `src_mode` 表明了图片资源是来自内存地址还是文件路径，如示例中展示，`src_mode` 为 `IMG_SRC_MEMADDR` 时表示图片资源来自内存地址。

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_perspective.h
   :language: c
   :start-after: /* perspective imgfile structure start*/
   :end-before: /* perspective imgfile structure end*/

```

### 设置透视控件的图片

使用 [void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file)](#gui_perspective_set_img) 函数设置透视控件的图片。

### 设置图片混合模式

默认情况下，控件的图像混合模式为 `IMG_SRC_OVER_MODE`，调用 [void gui_perspective_set_mode(gui_perspective_t *perspective, uint8_t img_index, BLEND_MODE_TYPE mode)](#gui_perspective_set_mode) 函数可以设置图像的混合模式。混合模式 `BLEND_MODE_TYPE` 枚举类型如下：

```eval_rst

.. literalinclude:: ../../../gui_engine/engine/draw_img.h
   :language: c
   :start-after: /* BLEND_MODE_TYPE structure start*/
   :end-before: /* BLEND_MODE_TYPE structure end*/

```

## 示例
<details> <summary>代码</summary>

```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_448_368/app_prism.c
   :language: c
   :start-after: /* perspective example start*/
   :end-before: /* perspective example end*/

```
</details>
<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1699931105543257223/ccac3ca0_10641540.png" width = "300" /></div>
<br>

<span id = "gui_perspective_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_perspective.h

```

# Perspective

The perspective is a hexagonal prism-like widget that uses six tabs as column faces. Once the perspective control is created, it automatically rotates to display different tabs. During the rotation, you can also click on different tabs to quickly jump to the desired tab.

## Usage

### Create Perspective Widget

[gui_perspective_t *gui_perspective_create(void *parent,  const char *name, gui_perspective_imgfile_t *img_file, int16_t x, int16_t y)](#gui_perspective_create) is used to create a perspective widget. The `img_file` is a structure that contains image resources for the six tabs. When creating the perspective widget, the image resources for the six tabs can be specified using either memory addresses or file paths. The `src_mode` field in `gui_perspective_imgfile_t` (refer to the following structure) indicates whether the image resource originates from a memory address or a file path. For instance, when `src_mode` is set to `IMG_SRC_MEMADDR`, it means the image resource comes from a memory address.

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_perspective.h
   :language: c
   :start-after: /* perspective imgfile structure start*/
   :end-before: /* perspective imgfile structure end*/

```

### Set Image for Perspective
The images of cube can be configured by calling [void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file)](#gui_perspective_set_img).

### Set Image Blending Mode

By default, the image blending mode is set to `IMG_SRC_OVER_MODE`, you can change the image blending mode by calling [void gui_perspective_set_mode(gui_perspective_t *perspective, uint8_t img_index, BLEND_MODE_TYPE mode)](#gui_perspective_set_mode). The `BLEND_MODE_TYPE` enumeration for the blending modes is as follows:

```eval_rst

.. literalinclude:: ../../../gui_engine/engine/draw_img.h
   :language: c
   :start-after: /* BLEND_MODE_TYPE structure start*/
   :end-before: /* BLEND_MODE_TYPE structure end*/

```

## Example

<details> <summary>code</summary>

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

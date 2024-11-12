# SVG

- The SVG widget allows you to display an image in SVG format.
- SVG images are vector graphics based on XML language implementation, which can be used to describe 2D vector graphics.
- SVG images **do not need to be converted into** `bin` format files using ImageConverter tools, but can be directly packaged in the `root` directory.

## Usage
### Creat Svg Widget
You can use [gui_svg_create_from_mem(void *parent, const char *name, uint8_t *addr, uint32_t size, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_svg_create_from_mem) to create an SVG widget from memory, or use [gui_svg_create_from_file(void *parent, const char *name, const char *filename, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_svg_create_from_file) to create from file.
This `w/h` are the width and height of the SVG widget, not SVG image. This `size` is the data size of the SVG image that must be filled in.

### SVG Rotation
Using the [gui_svg_rotation(gui_svg_t *svg, float degrees, float c_x, float c_y)](#gui_svg_rotation) the image will be rotated by an angle of `degree`. The `(c_x, c_y)` is relative to the origin of the SVG widget, not the origin of the screen.

### SVG Scale
If you need the object size to be updated to transformed size set [gui_svg_scale(gui_svg_t *svg, float scale_x, float scale_y)](#gui_svg_scale).

### SVG Translate
The image will be translated by using the [gui_svg_translate(gui_svg_t *svg, float t_x, float t_y)](#gui_svg_translate).

### Opacity
If you want the image to fade or opacity set [gui_svg_set_opacity(gui_svg_t *svg, unsigned char opacity_value)](#gui_svg_set_opacity).

## Example

### Creat Simple SVG
```cpp
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{ 
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
}
```
<br/>
<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1699598969684310669/2e2a68e7_13671125.png"/></div>
<br/>

### SVG Rotation

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{ 
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_rotation(svg1,90,50,50);
}
```
<br/>
<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1699598974541085137/fcc74440_13671125.png"/></div>
<br/>

### SVG Scale

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{ 
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_scale(svg1,2,1);
}
```
<br/>
<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1699598982107316449/af526d67_13671125.png"/></div>
<br/>

### SVG Translate

```c
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{ 
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
    gui_svg_translate(svg1,100,100);
}
```
<br/>
<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1699598986751661244/ff6ea9cf_13671125.png"/></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_svg.h

```

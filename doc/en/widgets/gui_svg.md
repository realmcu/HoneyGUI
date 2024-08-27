# SVG

- The SVG widget allows you to display an image in SVG format.
- SVG images are vector graphics based on XML language implementation, which can be used to describe 2D vector graphics.
- SVG images **do not need to be converted into** `bin` format files using ImageConverter tools, but can be directly packaged in the `root` directory.

## Usage
### Creat a svg widget
You can use [gui_svg_create_from_mem(parent, name, addr, size, x, y, w, h)](#gui_svg_create_from_mem) to create an SVG widget from memory, or use [gui_svg_create_from_file(parent, name, filename, x, y, w, h)](#gui_svg_create_from_file) to create from file.
This `w/h` are the width and height of the SVG widget, not SVG image. This `size` is the data size of the SVG image that must be filled in.

### SVG rotation
Using the [gui_svg_rotation(svg, degrees, c_x, c_y)](#gui_svg_rotation) the image will be rotated by an angle of `degree`. The `(c_x, c_y)` is relative to the origin of the SVG widget, not the origin of the screen.

### SVG scale
If you need the object size to be updated to transformed size set [gui_svg_scale(svg, scale_x, scale_y)](#gui_svg_scale).

### SVG translate
The image will be translated by using the [gui_svg_translate(svg, t_x, t_y)](#gui_svg_translate).

### Opacity
If you want the image to fade or opacity set [gui_svg_set_opacity(svg, opacity_value)](#gui_svg_set_opacity).

## Example

### Creat a simple SVG
```cpp
#include "gui_svg.h"
#include "root_image_hongkong/ui_resource.h"
void page_tb_svg(void *parent)
{ 
    gui_svg_t* svg1=gui_svg_create_from_mem(parent,"svg1",ACTIVITY_SVG,5184,0,0,100,100);
}
```
<br/>
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598969684310669/2e2a68e7_13671125.png"/></div>
<br/>

### SVG rotation

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
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598974541085137/fcc74440_13671125.png"/></div>
<br/>

### SVG scale

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
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598982107316449/af526d67_13671125.png"/></div>
<br/>

### SVG translate

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
<div style="text-align: center"><img width="300" src ="https://foruda.gitee.com/images/1699598986751661244/ff6ea9cf_13671125.png"/></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_svg.h

```

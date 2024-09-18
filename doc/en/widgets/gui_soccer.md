# 2.5D soccer

2.5D soccer consists of 12 vertices and 20 faces, which can form an approximately 3D spherical shape. Among them, the vertices define the shape and position of the sphere, and the face is a plane formed by connecting multiple vertices, used to fill the surface of the sphere.

## Usage
### Create widget
Use [gui_soccer_t *gui_soccer_create(void *parent, const char  *name, void *addr, int16_t x, int16_t y)](#gui_soccer_create) to create a 2.5D soccer. The `addr` parameter is the image used to fill the white faces of the soccer ball.

### Set size
The default distance between the vertices of the soccer is 100. You can change the size of the soccer by using [void gui_soccer_set_size(gui_soccer_t *this, float size)](#gui_soccer_set_size) to modify the distance between the vertices.

### Set center
The default center of the soccer is `((dc->fb_width - this->scsize) / 2.0f)`, where `this->scsize` is the distance between the vertices of the soccer. You can change the center coordinates of the soccer by using [void gui_soccer_set_center(gui_soccer_t *this, float c_x, float c_y)](#gui_soccer_set_center).

### Set image mode
The default image blending mode for the soccer is  `IMG_SRC_OVER_MODE`. You can change the image blending mode by using [void gui_soccer_set_mode(gui_soccer_t *soccer, BLEND_MODE_TYPE mode)](#gui_soccer_set_mode).

### Set image opacity
You can change the opacity of the image by using [void gui_soccer_set_opacity(gui_soccer_t *soccer, uint8_t opacity)](#gui_soccer_set_opacity).

### Set image for soccer faces
You can set the image for the soccer ball faces by using [void gui_soccer_set_img(gui_soccer_t *soccer, void *addr)](#gui_soccer_set_img).


## Example


```c
#include "gui_app.h"
#include "gui_soccer.h"
#include "green_hexagon.txt"

static void app_dialing_ui_design(gui_app_t *app)
{
    gui_soccer_t *soccer = gui_soccer_create(&(app->screen), "soccer", (void *)_acgreen_hexagon, 0, 0);
    // gui_soccer_set_size(soccer, 80);
    gui_soccer_set_center(soccer, 240, 240);
}
```
<br/>
<div style="text-align: center"><img width= "400" src="https://docs.realmcu.com/HoneyGUI/image/widgets/soccer.gif"></div>
<br/>

## API

```eval_rst

.. doxygenfile:: gui_soccer.h

```

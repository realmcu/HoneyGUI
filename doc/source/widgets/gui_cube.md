# cube
The cube widget can display 6 tabs as a rotating cube.


## demo
```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/10nRpfLmNMgtpwJrH8JEjicDHG-QCrrU9/preview" width="640" height="480" allow="autoplay"></iframe>
```
```c
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"

void page_tb_cube(void *parent)
{
    uint8_t *array[6] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};


    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube4, 100);
    gui_cube_set_center(cube4, 200, 200);
}
```

## API
```eval_rst
.. doxygenfunction:: gui_cube_create
.. doxygenfunction:: gui_cube_set_center
.. doxygenfunction:: gui_cube_set_size
```

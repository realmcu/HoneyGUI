# dynamic

make dynamic frame

## demo
```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1tq2wCY16waEF7jcq0R3DhCDXSg5pRSvH/preview" width="640" height="480" allow="autoplay"></iframe>
```



```

#include "rtk_gui_resource.h"
#include <gui_app.h>
#include <gui_magic_img.h>
#include <gui_dynamic_img.h>

#define TIME_DY 50
gui_dynamic_img_t *action_d;

void *action_list[24] =
{
    CHARGE10_BIN, CHARGE12_BIN, CHARGE14_BIN, CHARGE16_BIN, CHARGE18_BIN, CHARGE20_BIN,
    CHARGE22_BIN, CHARGE24_BIN, CHARGE26_BIN, CHARGE28_BIN, CHARGE30_BIN, CHARGE32_BIN,
    CHARGE34_BIN, CHARGE36_BIN, CHARGE38_BIN, CHARGE40_BIN, CHARGE42_BIN, CHARGE44_BIN,
    CHARGE46_BIN, CHARGE48_BIN, CHARGE50_BIN, CHARGE52_BIN, CHARGE54_BIN, CHARGE56_BIN,
};

void dynamic_create(void *parent)
{
    action_d = gui_dynamic_create_from_mem(parent, "speed", action_list, 150,
                                                              150, 24, TIME_DY * 2,
                                                              TIME_DY * 1000);
}
void dynamic_restart(void *parent)
{
    gui_dynamic_img_restart(action_d);
}
void dynamic_stop(void *parent)
{
    gui_dynamic_img_stop(action_d);
}
void dynamic_start(void *parent)
{
    gui_dynamic_img_start(action_d);
}
void dynamic_ui_design(gui_app_t *app)
{
    gui_obj_t *scr = &(app->screen);
   
    dynamic_create(scr);
    dynamic_stop(scr);
    dynamic_restart(scr);
}

```
## API
```eval_rst
.. doxygenfunction:: gui_dynamic_create_from_mem
create a dynamic widget
```


```eval_rst
.. doxygenfunction:: gui_dynamic_img_stop
.. doxygenfunction:: gui_dynamic_img_start
.. doxygenfunction:: gui_dynamic_img_restart

```



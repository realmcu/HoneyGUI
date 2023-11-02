# tabview

Please refer to the page below:



```eval_rst
`tab`_    

.. _tab: gui_tab.html
```

```
#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_win.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include <gui_magic_img.h>
#include "app_hongkong.h"


gui_tabview_t *tv;

static void tab_ui_design(gui_app_t *app)
{
    gui_log("tab_ui_design\n");

    tv = gui_tabview_create(&(app->screen), "tabview", 0, 0, 0, 0);

    gui_tab_t *tb_clock = gui_tab_create(tv, "tb_clock",           0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_activity = gui_tab_create(tv, "tb_activity",     0, 0, 0, 0, 1, 0);
    gui_tab_t *tb_heart = gui_tab_create(tv, "tb_heart",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tb_weather = gui_tab_create(tv, "tb_weather",       0, 0, 0, 0, 4, 0);
    gui_tab_t *tb_music = gui_tab_create(tv, "tb_music",           0, 0, 0, 0, 3, 0);
    page_tb_clock(tb_clock);
    gui_tab_set_style(tb_clock,REDUCTION);
    page_tb_activity(tb_activity);
    gui_tab_set_style(tb_activity,REDUCTION);
    page_tb_heart(tb_heart);
    gui_tab_set_style(tb_heart,REDUCTION);
    page_tb_weather(tb_weather);
    page_tb_music(tb_music);
}



```

## API

```eval_rst
.. doxygenfunction:: gui_tabview_create
.. doxygenfunction:: gui_tabview_jump_tab
.. doxygenfunction:: gui_tabview_set_style

```




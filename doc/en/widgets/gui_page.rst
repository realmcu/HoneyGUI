=====
Page
=====

The page widget is a container widget that allow developers to create a page containing other widgets along the y-axis. After creating a page, various other widgets can be added to it, such as button, image, and more. The page widget can extend beyond the screen display boundaries, and users can access additional controls added to the page by swiping up or down. 

Usage
-----
Create Page Widget
~~~~~~~~~~~~~~~~~~~
Developers can utilize the :cpp:any:`gui_page_create` function to establish a page widget. The page widget is a container oriented vertically, allowing the addition of other widgets to it. The height of the page widget is determined by the number of widgets added to it. As more widgets are added, its height increases.

Example
-------

.. code-block:: c

    #include "root_image_hongkong/ui_resource.h"
    #include <gui_img.h>
    #include "gui_win.h"
    #include "gui_text.h"
    #include <draw_font.h>
    #include "gui_button.h"
    #include "gui_page.h"

    extern void callback_prism(void *obj, gui_event_t e);

    void page_tb_activity(void *parent)
    {
        gui_img_create_from_mem(parent, "page1", ACTIVITY_BIN, 0, 0, 0, 0);
        gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
        gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);

        gui_page_t *pg1 = gui_page_create(parent, "page", 0, 0, 0, 0);

        gui_img_t *img1 = gui_img_create_from_mem(pg1, "img1", BUTTON1_BIN, 0, 150, 0, 0);
        gui_img_t *img2 = gui_img_create_from_mem(pg1, "img2", BUTTON2_BIN, 0, 230, 0, 0);
        gui_img_t *img3 = gui_img_create_from_mem(pg1, "img3", BUTTON3_BIN, 0, 300, 0, 0);
        gui_img_t *img4 = gui_img_create_from_mem(pg1, "img4", PLAYER_MUSIC_REWIND_ICON_BIN, 0, 380, 0, 0);
        gui_img_t *img5 = gui_img_create_from_mem(pg1, "img5", PLAYER_MUSIC_WIND_ICON_BIN, 0, 460, 0, 0);
    }


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/page.gif" width= "400" /></div>
   <br>


API
-----

.. doxygenfile:: gui_page.h


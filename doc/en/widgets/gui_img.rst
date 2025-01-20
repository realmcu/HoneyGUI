===
Img
===

The image widget is the basic widget used to display images. Image widgets support moving, zooming, rotating, etc.

Usage
-----

Create Widget
~~~~~~~~~~~~~

It is possible to use :cpp:any:`gui_img_create_from_mem` to create an image widget from memory, or use :cpp:any:`gui_img_create_from_fs` to create an image widget from a file. Alternatively, :cpp:any:`gui_img_create_from_ftl` can be used to create an image widget from ftl.
If the width or height of the image widget is set to `0`, the widget's size will be set according to the size of the image source automatically.

Update Location
~~~~~~~~~~~~~~~

If it is necessary to update the location of an image widget, use :cpp:any:`gui_img_set_location` to relocate.

Set Attribute
~~~~~~~~~~~~~

It is possible to use :cpp:any:`gui_img_set_attribute` to set the attribute of an image widget, replace it with a new image, and set a new coordinate.

Get Height/Width
~~~~~~~~~~~~~~~~

If you want to get the height/width of image widget, do so with :cpp:any:`gui_img_get_height` or :cpp:any:`gui_img_get_width`.

Refresh
~~~~~~~

Refresh the image size using :cpp:any:`gui_img_refresh_size`.

Blend Mode
~~~~~~~~~~

Set the image's blend mode using :cpp:any:`gui_img_set_mode`.

Translation
~~~~~~~~~~~

Use :cpp:any:`gui_img_translate` to move the image widget.
It can move an image widget to a new coordinate without changing the original coordinate in the widget's attribute.

Rotation
~~~~~~~~

Rotate the image widget around the center of the circle with this API :cpp:any:`gui_img_rotation`.

Zoom
~~~~

You can adjust the size of the image widget to fit your requirements by this API :cpp:any:`gui_img_scale`.

Opacity
~~~~~~~

The opacity value of the image is adjustable, and it can be set using :cpp:any:`gui_img_set_opacity`.

Animation
~~~~~~~~~

The :cpp:any:`gui_img_set_animate` can be used to set the animation effects for the image widget.

Quality
~~~~~~~

The image's quality can be set using :cpp:any:`gui_img_set_quality`.

Screenshot
~~~~~~~~~~

The :cpp:any:`gui_img_tree_convert_to_img` can be used to save a fullscreen screenshot. Additionally, the :cpp:any:`gui_img_tree_convert_to_img_root_size` can be used to save a screenshot of the current widget's size. The saved image will be in RGB format.

Example
-------

.. code-block:: c

    #include "root_image_hongkong/ui_resource.h"
    #include "gui_img.h"
    #include "gui_text.h"
    #include "draw_font.h"

    char *tb1_text = "gui_img_create_from_mem";

    void page_tb1(void *parent)
    {
        static char array1[50];
        static char array2[50];

        gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);

        gui_text_t *text1 = gui_text_create(parent, "text1", 10, 100, 300, 30);
        gui_text_set(text1, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
        gui_text_mode_set(text1, LEFT);

        gui_text_t *text2 = gui_text_create(parent, "text2", 10, 130, 330, 30);
        gui_text_set(text2, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
        gui_text_mode_set(text2, LEFT);
        sprintf(array1, "gui_img_get_height %d", gui_img_get_height(img_test));
        text2->utf_8 = array1;
        text2->len = strlen(array1);

        gui_text_t *text3 = gui_text_create(parent, "text3", 10, 160, 330, 30);
        gui_text_set(text3, tb1_text, GUI_FONT_SRC_BMP, 0xffffffff, strlen(tb1_text), 24);
        gui_text_mode_set(text3, LEFT);
        sprintf(array2, "gui_img_get_width %d", gui_img_get_width(img_test));
        text3->utf_8 = array2;
        text3->len = strlen(array2);
    }

    void page_tb2(void *parent)
    {
        gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
        gui_img_set_location(img_test, 50, 50);

        gui_text_t *text2 = gui_text_create(parent, "text2", 10, 100, 330, 24);
        gui_text_set(text2, "gui_img_set_location", GUI_FONT_SRC_BMP, 0xffffffff, 20, 24);
        gui_text_mode_set(text2, LEFT);
    }

    void page_tb3(void *parent)
    {
        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
        gui_img_set_attribute(img_test, "test", SET_OFF_BIN, 20, 20);

        gui_text_t *text3 = gui_text_create(parent, "text3", 10, 100, 330, 24);
        gui_text_set(text3, "gui_img_set_attribute", GUI_FONT_SRC_BMP, 0xffffffff, 21, 24);
        gui_text_mode_set(text3, LEFT);

    }

    void page_tb4(void *parent)
    {
        gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
        gui_img_scale(img_test, 0.5, 0.5);

        gui_text_t *text4 = gui_text_create(parent, "text4", 10, 100, 330, 24);
        gui_text_set(text4, "gui_img_scale", GUI_FONT_SRC_BMP, 0xffffffff, 13, 24);
        gui_text_mode_set(text4, LEFT);
    }

    void page_tb5(void *parent)
    {
        gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
        gui_img_translate(img_test, 100, 100);

        gui_text_t *text5 = gui_text_create(parent, "text5", 10, 100, 330, 24);
        gui_text_set(text5, "gui_img_translate", GUI_FONT_SRC_BMP, 0xffffffff, 17, 24);
        gui_text_mode_set(text5, LEFT);
    }

    void page_tb6(void *parent)
    {
        gui_set_font_mem_resourse(24, TEST_FONT24_DOT_BIN, TEST_FONT24_TABLE_BIN);

        gui_img_t *img_test = gui_img_create_from_mem(parent, "test", SET_ON_BIN, 0, 0, 0, 0);
        gui_img_rotation(img_test, 10, 0, 0);

        gui_text_t *text6 = gui_text_create(parent, "text6", 10, 100, 330, 24);
        gui_text_set(text6, "gui_img_rotation", GUI_FONT_SRC_BMP, 0xffffffff, 16, 24);
        gui_text_mode_set(text6, LEFT);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/img.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_img.h

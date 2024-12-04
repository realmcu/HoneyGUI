=======
STB Img
=======

The image widget is the basic widget used to display an image. You can set left, right, and center images, and the center image will be repeated to match the width of the object. STB image widgets support bmp, jpg, png, gif, etc.

In the STB image library, STB is not an acronym. It is actually a naming convention used to identify a series of single-file libraries developed by Sean T. Barrett. The STB image library ``stb_image.h`` is one of these libraries and is used to load and store various common image formats.

Usage
-----

Create Widget
~~~~~~~~~~~~~~

You can use :cpp:any:`gui_img_stb_create_from_mem` to create an STB image widget from memory. Please confirm that the type and size are correct.

Set Attribute
~~~~~~~~~~~~~~

You can use :cpp:any:`gui_img_stb_set_attribute` to set the attribute of an STB image widget, replace it with a new file and set a new coordinate.

Example
-------

.. code-block:: c

    #include "root_image_hongkong/ui_resource.h"
    #include "gui_obj.h"
    #include "gui_app.h"
    #include "gui_img.h"
    #include "gui_img_stb.h"

    static void app_home_ui_design(gui_app_t *app)
    {
        gui_stb_img_t *jpg = gui_img_stb_create_from_mem(&app->screen, "jpg", TEST_JPG, 0x6640, JPEG, 0, 0);
        gui_stb_img_t *png = gui_img_stb_create_from_mem(&app->screen, "png", TEST_PNG, 0x2B00, PNG, 170, 170);
    }

.. figure:: https://foruda.gitee.com/images/1703146027234656357/48137b9c_9325830.png
   :align: center
   :width: 400px

   STB Image Example

API
---

.. doxygenfile:: gui_img_stb.h
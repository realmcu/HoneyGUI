============
Cube
============

The cube is a 3D display widget that show a rotating cube with six faces, each capable of displaying distinct images.

Usage
-----

Create Cube
~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:any:`gui_cube_create` creates a cube widget. The ``img_file`` is an struct including image sources for the six faces of a cube. Both memory address and file path are supported, whether you choose to utilize memory or access the filesystem, simply set ``IMG_SOURCE_MODE_TYPE src_mode[6]`` to either ``IMG_SRC_MEMADDR`` or ``IMG_SRC_FILESYS``.

Set Size
~~~~~~~~~~~~~~~~~~~~~~~~~

By default, the cube's size is ``dc->fb_height / 8.0``. If you want to change the size set :cpp:any:`gui_cube_set_size`. Note this ``size`` is the 1/2 length of cube edge.

Set Center
~~~~~~~~~~~~~~~~~~~~~~~~~

By default, the center is ``((dc->fb_width - size) / 2.0f,dc->fb_width - size) / 2.0f)``. You can use :cpp:any:`gui_cube_set_center` to set the center coordinates of the cube.

Set Image Mode
~~~~~~~~~~~~~~~~~~~~~~~~~

By default, the cube's image blend mode is ``IMG_FILTER_BLACK``, you can change the blend mode of image by calling :cpp:any:`gui_cube_set_mode`.

Set Cube Image
~~~~~~~~~~~~~~~~~~~~~~~~~

The images of cube can be configured by calling :cpp:any:`gui_cube_set_img`.

Set Rotation Direction
~~~~~~~~~~~~~~~~~~~~~~~~~

The rotation direction of this cube can be achieved by calling :cpp:any:`gui_cube_auto_rotation_by_x`, :cpp:any:`gui_cube_auto_rotation_by_y` and :cpp:any:`gui_cube_auto_rotation_by_z`.


Example
-------

.. code-block:: c

    #include <gui_obj.h>
    #include "gui_cube.h"
    #include "root_image_hongkong/ui_resource.h"

    void page_tb_cube(void *parent)
    {
        gui_cube_imgfile_t imgfile =
        {
            .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
            .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
            .data_addr.data_addr_front = ACTIVITY_BIN,
            .data_addr.data_addr_back = WEATHER_BIN,
            .data_addr.data_addr_up =  HEARTRATE_BIN,
            .data_addr.data_addr_down = CLOCKN_BIN,
            .data_addr.data_addr_left = MUSIC_BIN,
            .data_addr.data_addr_right = QUICKCARD_BIN
        };
        gui_cube_t *cube4 = gui_cube_create(parent, "cube", &imgfile, 0, 0);
        gui_cube_auto_rotation_by_y(cube4, 100, 5.5f);
        gui_cube_set_mode(cube4, CUBE_SIDE_DOWN, IMG_SRC_OVER_MODE);
        gui_cube_set_size(cube4, 100);
        gui_cube_set_center(cube4, 200, 200);
    }


.. raw:: html

    <br/>
    <div style="text-align: center"><img width= "400" src="https://docs.realmcu.com/HoneyGUI/image/widgets/cube.gif"></div>
    <br/>

API
-----

.. doxygenfile:: gui_cube.h


.. _Widget_Wheel_List_EN:

===========
Wheel List
===========

The wheel list widget is an application list interface, mainly composed of application icons arranged in multiple circular layers.
Each layer can rotate independently, and each icon can have an associated callback.

Usage
-----

Create a Wheel List
~~~~~~~~~~~~~~~~~~~

:cpp:any:`gui_wheel_list_create` is used to create a heel list widget.

``layer`` is the number of layers of the round wheel, 1 means there is one circle of outer icons in addition to the center icon.

``radius`` is the radius of the icon. If the icon is square, it is half of the side length of the icon image.
This value needs to be accurate, otherwise errors will occur in the icon arrangement calculation and position calculation.

``distance`` is the distance between the centers of the icons of two adjacent layers,
which needs to be greater than the diameter or side length of the icon.

In Layer 0, there is a central icon, whose center is at the center of the screen.
This point serves as the reference for calculating the positions of other icons.
The number of icons in each layer is 6*n, where n represents the number of icon layers.
Therefore, the closer to the outer layer, the more icons there are,
and the smaller the angle between the centers of adjacent icons and the center of the screen on the same layer.

Add Icon
~~~~~~~~

Default
-------

After creating the wheel list widget, you can use the default interface for adding icons.
In the wheel, corresponding icons can be added from the inside out in a layer-by-layer fashion.

In :cpp:any:`gui_wheel_list_add_icon_default`

``this`` is the pointer to the widget.
``icon_addr`` is the pointer to the address of the image.
``event_cb`` is the callback function of this image, triggered by clicking.

.. code-block:: c

    #include "gui_wheel_list.h"
    #include "gui_img.h"
    #include "gui_app.h"
    #include "root_image_8762g/ui_resource.h"

    static void app_launcher_ui_design(gui_app_t *app);
    static gui_app_t app_launcher =
    {
        .screen =
        {
            .name = "app_launcher",
            .x    = 0,
            .y    = 0,
        },
        .ui_design = app_launcher_ui_design,
        .active_ms = 1000 * 5,
    };
    gui_app_t *get_launcher_app(void)
    {
        return &app_launcher;
    }

    static void app_wheel_ui_design(gui_app_t *app);
    static gui_app_t app_wheel =
    {
        .screen =
        {
            .name = "app_wheel",
            .x    = 0,
            .y    = 0,
        },
        .ui_design = app_wheel_ui_design,
        .active_ms = 1000 * 5,
    };
    gui_app_t *get_wheel_app(void)
    {
        return &app_wheel;
    }

    static void wheel_cb(void *obj, gui_event_t event)
    {

    }
    static void switch_to_launcher(void *obj, gui_event_t event)
    {
        gui_switch_app(get_wheel_app(), get_launcher_app());
    }
    static void app_wheel_ui_design(gui_app_t *app)
    {
        gui_wheel_list_t *hc = gui_wheel_list_create(&app->screen, 2, 32, 80);
        gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, switch_to_launcher);

        gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);
        gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, wheel_cb);
        gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, wheel_cb);
        gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, wheel_cb);
        gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, wheel_cb);
        gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, wheel_cb);

        gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUMUSIC_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUALARM_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUBIRD_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUALBUM_BIN, NULL);
        gui_wheel_list_add_icon_default(hc, ICMENUHEARTRATE_BIN, NULL);
    }

    static void app_launcher_ui_design(gui_app_t *app)
    {

    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/wheel_list1.gif" width= "456" /></div>
   <br>

Default Wheel List Demo

Customized
----------

After creating the wheel list widget, you can use the default interface for adding icons.
In the wheel, corresponding icons can be added from the inside out in a layer-by-layer fashion.

In :cpp:any:`gui_wheel_list_set_icon`

``this`` is the pointer to the widget.
``icon_addr`` is the pointer to the address of the image.
``event_cb`` is the callback function of this image, triggered by clicking.
``layer`` is the level or layer where the icon is located.

.. code-block:: c

    #include "gui_wheel_list.h"
    #include "gui_img.h"
    #include "gui_app.h"
    #include "root_image_8762g/ui_resource.h"

    static void app_launcher_ui_design(gui_app_t *app);
    static gui_app_t app_launcher =
    {
        .screen =
        {
            .name = "app_launcher",
            .x    = 0,
            .y    = 0,
        },
        .ui_design = app_launcher_ui_design,
        .active_ms = 1000 * 5,
    };
    gui_app_t *get_launcher_app(void)
    {
        return &app_launcher;
    }

    static void app_wheel_ui_design(gui_app_t *app);
    static gui_app_t app_wheel =
    {
        .screen =
        {
            .name = "app_wheel",
            .x    = 0,
            .y    = 0,
        },
        .ui_design = app_wheel_ui_design,
        .active_ms = 1000 * 5,
    };
    gui_app_t *get_wheel_app(void)
    {
        return &app_wheel;
    }

    static void wheel_cb(void *obj, gui_event_t event)
    {

    }
    static void switch_to_launcher(void *obj, gui_event_t event)
    {
        gui_switch_app(get_wheel_app(), get_launcher_app());
    }
    static void app_wheel_ui_design(gui_app_t *app)
    {
        gui_wheel_list_t *hc = gui_wheel_list_create(&app->screen, 2, 32, 80);
        gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, switch_to_launcher, 0, 0);

        gui_wheel_list_set_icon(hc, ICMENUBIRD_BIN, wheel_cb, 1, 0);
        gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, wheel_cb, 1, 2);
        gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, wheel_cb, 1, 4);

        gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 1);
        gui_wheel_list_set_icon(hc, ICMENUMUSIC_BIN, NULL, 2, 2);
        gui_wheel_list_set_icon(hc, ICMENUALARM_BIN, NULL, 2, 3);
        gui_wheel_list_set_icon(hc, ICMENUALBUM_BIN, NULL, 2, 5);
        gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 6);
        gui_wheel_list_set_icon(hc, ICMENUMUSIC_BIN, NULL, 2, 7);
        gui_wheel_list_set_icon(hc, ICMENUBIRD_BIN, NULL, 2, 9);
        gui_wheel_list_set_icon(hc, ICMENUALBUM_BIN, NULL, 2, 10);
        gui_wheel_list_set_icon(hc, ICMENUHEARTRATE_BIN, NULL, 2, 11);
    }

    static void app_launcher_ui_design(gui_app_t *app)
    {

    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/wheel_list2.gif" width= "456" /></div>
   <br>

Customized Wheel List Demo

API
---

.. doxygenfile:: gui_wheel_list.h

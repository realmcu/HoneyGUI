.. _Glass:

=====
Glass
=====

The glass widget is used to simulate the refraction effect of glass on a screen. Glass widgets can move on screen and requires an entire frame buffer.

To reduce time consumption in HoneyGUI, a simplified glass model which doesn't follow the principles of light refraction is applied. The simplified glass model only includes the the difference between the current coordinates and the refracted coordinates, as illustrated in the diagram below:

.. figure:: https://foruda.gitee.com/images/1760423436149037567/32e6dd13_8391010.jpeg
   :width: 400px
   :align: center
   
   Simplified Model 

Usage
-----

.. table:: Gui_Glass table
   :widths: 100 100
   :align: center
   :name: Gui_Glass_Table

   +------------------------------------------------------+---------------------------------------------------+
   | Description                                          | API                                               |
   +======================================================+===================================================+
   | Create Widget                                        | :cpp:any:`gui_glass_effect_create_from_mem`       |
   +------------------------------------------------------+---------------------------------------------------+
   | Set Attrei                                           | :cpp:any:`gui_glass_effect_set_attribute`         |
   +------------------------------------------------------+---------------------------------------------------+
   | Obtain Height                                        | :cpp:any:`gui_glass_effect_get_height`            |
   +------------------------------------------------------+---------------------------------------------------+
   | Obtain Width                                         | :cpp:any:`gui_glass_effect_get_width`             |
   +------------------------------------------------------+---------------------------------------------------+
   | Refresh Attribute                                    | :cpp:any:`gui_glass_effect_refresh_size`          |
   +------------------------------------------------------+---------------------------------------------------+
   | Posistion Translation                                | :cpp:any:`gui_glass_effect_translate`             |
   +------------------------------------------------------+---------------------------------------------------+


Example
-------

.. literalinclude:: ../../../example/widget/glass_effect/example_glass_effect.c
    :language: c
    :start-after: /* gui glass widget example start*/
    :end-before: /* gui glass widget example end*/

    

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1760426386384113377/a1a16aae_8391010.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_glass.h

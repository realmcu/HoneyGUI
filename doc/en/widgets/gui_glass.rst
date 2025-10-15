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

Glass Data Generation
----------------------

Based on the simplified model above, the rule can be derived that the offset of the pixel points after refraction is only related to the pixels within the range of the glass. Therefore, only a collection of the offset of each point on the glass is needed. In the design of the :ref:example <glass_example_en> , the offset is determined by the thickness(refractive) of the glass and the position of the observing point (x_o, y_o) through the following relationship:

.. math::

offset(x, y) = distortion(x, y) * d(x, y)

where distortion represents the thickness (refractive index) of each point on the glass, and d(x, y) represents the relationship between the current point on the glass and the observed point (x_o, y_o).

Usage
-----

.. table:: Gui_Glass table
   :widths: 100 100
   :align: center
   :name: Gui_Glass_Table

   +------------------------------------------------------+---------------------------------------------------+
   | Description                                          | API                                               |
   +======================================================+===================================================+
   | Create Widget                                        | :cpp:any:`gui_glass_create_from_mem`       |
   +------------------------------------------------------+---------------------------------------------------+
   | Set Attrei                                           | :cpp:any:`gui_glass_set_attribute`         |
   +------------------------------------------------------+---------------------------------------------------+
   | Obtain Height                                        | :cpp:any:`gui_glass_get_height`            |
   +------------------------------------------------------+---------------------------------------------------+
   | Obtain Width                                         | :cpp:any:`gui_glass_get_width`             |
   +------------------------------------------------------+---------------------------------------------------+
   | Refresh Attribute                                    | :cpp:any:`gui_glass_refresh_size`          |
   +------------------------------------------------------+---------------------------------------------------+
   | Posistion Translation                                | :cpp:any:`gui_glass_translate`             |
   +------------------------------------------------------+---------------------------------------------------+

.. _glass_example_en:
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

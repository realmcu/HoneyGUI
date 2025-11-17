.. _Circle (Geometry Circle):

Geometry Circle
========================

Overview
--------

The Circle widget is a lightweight :term:`GUI` drawing component designed specifically for rendering circle shapes in user interfaces. This component provides a simple and easy-to-use :term:`API` for customizing the circle's center position, radius, color, etc., allowing the creation of various circular :term:`UI` elements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763463135933874666/f2c5f5f8_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_lite_circle_create`  
   * - Set Style
     - :cpp:any:`gui_lite_circle_set_style`
   * - Set Position
     - :cpp:any:`gui_lite_circle_set_position` 
   * - Set Radius
     - :cpp:any:`gui_lite_circle_set_radius`
   * - Set Color
     - :cpp:any:`gui_lite_circle_set_color`
   * - Register Click Event Callback
     - :cpp:any:`gui_lite_circle_on_click`

Circle Characteristics
----------------------

The circle widget has the following geometric characteristics:

- **Center Positioning**: Precisely locate through center point coordinates
- **Radius Control**: Support any size radius value
- **Perfect Circle**: Ensure perfect geometric circular rendering
- **Boundary Handling**: Automatically handle clipping with boundaries

Feature Highlights
------------------

- **High Performance**: Utilizes optimized circle drawing algorithms to ensure smooth rendering performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth circular edge effects
- **Flexible Configuration**: Supports custom radius, position, and color
- **Transparency Support**: Supports ARGB color format for semi-transparent circle effects
- **Lightweight**: Minimal memory usage, suitable for embedded systems and resource-limited environments
- **Dynamic Updates**: Allows for runtime dynamic modification of position and style

Use Cases
---------

The circle widget is suitable for the following scenarios:

- **Status Indicators**: Create circular status indicator lights
- **User Avatar**: Implement circular user avatar display
- **Icon Design**: Draw various circular icon elements
- **Progress Indication**: Create circular progress bars or loading animations
- **Button Design**: Implement circular interactive buttons
- **Decorative Elements**: Serve as decorative circular elements in the UI
- **Data Visualization**: Used in pie charts, radar charts, etc., for data visualization components

Configuration Instructions
--------------------------

To use the circle widget, enable the corresponding macro definition in the configuration file:

In :file:`menu_config.h`, add:

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_CIRCLE 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/lite_geometry_circle/example_gui_lite_geometry_circle.c
   :language: c
   :start-after: /* gui lite geometry circle example start */
   :end-before: /* gui lite geometry circle example end */

API
---

.. doxygenfile:: gui_lite_geometry_circle.h

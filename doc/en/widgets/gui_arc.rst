.. _geometry_arc:

Geometry Arc
============

Overview
--------

The Geometry Arc widget is a lightweight :term:`GUI` drawing component designed for rendering arcs and circular shapes in user interfaces. It provides a simple and user-friendly :term:`API` to customize arc properties such as start angle, end angle, line width, and color, addressing various arc drawing requirements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1766129069269773340/db7572e4_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_arc_create`  
   * - Set Position
     - :cpp:any:`gui_arc_set_position` 
   * - Set Radius
     - :cpp:any:`gui_arc_set_radius`
   * - Set Color
     - :cpp:any:`gui_arc_set_color`
   * - Set Start Angle
     - :cpp:any:`gui_arc_set_start_angle`
   * - Set End Angle
     - :cpp:any:`gui_arc_set_end_angle`
   * - Set Line Width
     - :cpp:any:`gui_arc_set_line_width`
   * - Register Click Event Callback
     - :cpp:any:`gui_arc_on_click`
   * - Rotation Transform
     - :cpp:any:`gui_arc_rotate`
   * - Scale Transform
     - :cpp:any:`gui_arc_scale`
   * - Translation Transform
     - :cpp:any:`gui_arc_translate`

Angle Description
-----------------

The arc uses a standard mathematical coordinate system for angles:

- **0°**: Points to 3 o'clock (due east)
- **90°**: Points to 6 o'clock (due south)
- **180°**: Points to 9 o'clock (due west)
- **270°**: Points to 12 o'clock (due north)

Features Highlights
-------------------

- **High Performance**: Utilizes optimized rendering algorithms for smooth performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth visual effects
- **Flexible Configuration**: Allows customization of angle range, line width, and color
- **Lightweight**: Minimal memory footprint, suitable for embedded systems and resource-constrained environments
- **Ring Support**: Automatically draws a full circle when start and end angles differ by 360°
- **Matrix Transforms**: Supports rotation, scale, and translation matrix transforms for complex arc effects

Use Cases
---------

The arc widget is useful in scenarios such as:

- **Progress Indicators**: Displaying loading progress, battery levels, etc.
- **Dashboard Gauges**: Constructing speedometers, thermometers, etc.
- **Data Visualization**: Showing proportional data, statistical charts
- **UI Decoration**: As decorative elements in the interface
- **State Indicators**: Representing system statuses, connection statuses, etc.
- **Transform Animations**: Leverage rotation, scale, and translation for complex arc animation effects

Configuration Instructions
--------------------------

To use the arc widget, enable the corresponding macro in the configuration file:

In :file:`menu_config.h`, add:

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_ARC 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/arc/example_gui_arc.c
   :language: c
   :start-after: /* gui arc example start */
   :end-before: /* gui arc example end */

API
-------------

.. doxygenfile:: gui_arc.h


.. _Rounded_Rectangle:

Geometry Rounded Rectangle
=============================

Overview
--------

The Rounded Rectangle widget is a lightweight :term:`GUI` drawing component specifically designed for rendering rectangles with rounded corners in user interfaces. It offers a simple and user-friendly :term:`API` that allows customization of the rounded rectangle's size, corner radius, and color, enabling the creation of modern :term:`UI` elements.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1763457468837106189/45d195d0_13406851.png" width= "400" /></div>
   <br>

Core Features
-------------

.. list-table::
   :header-rows: 1

   * - Description
     - API 
   * - Create Widget
     - :cpp:any:`gui_lite_round_rect_create`  
   * - Set Style
     - :cpp:any:`gui_lite_round_rect_set_style`
   * - Set Position
     - :cpp:any:`gui_lite_round_rect_set_position` 
   * - Set Size
     - :cpp:any:`gui_lite_round_rect_set_size`
   * - Set Color
     - :cpp:any:`gui_lite_round_rect_set_color`
   * - Register Click Event Callback
     - :cpp:any:`gui_lite_round_rect_on_click`

Corner Radius Description
-------------------------

The rounded rectangle supports the same corner radius for all four corners:

- **radius = 0**: Draw a rectangle with sharp corners
- **radius > 0**: Draw a rounded rectangle, with the radius determining the arc of the corners


Feature Highlights
------------------

- **High Performance**: Utilizes optimized rendering algorithms for smooth performance
- **Anti-Aliasing**: Supports anti-aliasing for smooth rounded corner effects
- **Flexible Configuration**: Allows customization of size, corner radius, and color
- **Transparency Support**: Uses ARGB color format for semi-transparent effects
- **Lightweight**: Minimal memory footprint, suitable for embedded systems and resource-constrained environments
- **Dynamic Updates**: Supports runtime dynamic modification of position and style

Use Cases
---------

Rounded Rectangle widgets are applicable in scenarios such as:

- **Modern Buttons**: Create interactive buttons with rounded corner effects
- **Card Layouts**: Implement card-style UI design elements
- **Dialogs and Panels**: Render rounded corner dialogs and information panels
- **Icon Backgrounds**: Serve as background for app icons or functional icons
- **Progress Bar Containers**: Create rounded containers for progress bars
- **List Items**: Implement rounded list items or menu items
- **Animated Elements**: Support animation effects with dynamic position changes

Configuration Instructions
--------------------------

To use the rounded rectangle widget, enable the corresponding macro in the configuration file:

In :file:`menu_config.h`, add:

.. code-block:: c

   #define CONFIG_REALTEK_BUILD_REAL_LITE_RECT 1

Complete Example
----------------

.. literalinclude:: ../../../example/widget/lite_geometry_rect/example_gui_lite_geometry_rect.c
   :language: c
   :start-after: /* gui lite geometry rect example start */
   :end-before: /* gui lite geometry rect example end */

API 
-------------


.. doxygenfile:: gui_lite_geometry_round_rect.h

